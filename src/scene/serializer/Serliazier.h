 #pragma once
#include <string_view>
#include "../debugger/Debugger.h"
#include "../utils/Buffer.h"
#include "../utils/CRC32.h"
/* SERIALIZER
4 bytes - IRIS tag
8 bytes - size_t / amount of saved meta datas
SERIALIZER HEADER
varying - string / name of data including null byte
8 bytes - size_t / size of data without name of data
varying - string / type name
varying - string / property name including null byte
8 bytes - size_t / amount in bytes to skip to next property
8 bytes - size_t / sizeof data type of the property
varying - bytes  / raw data of property
1 byte  - null   / padding
LAST 4 BYTES OF FILE
4 bytes - u32    / CRC32
*/
namespace Iris {
	template <typename C, typename T>
	inline size_t constexpr offset_of(T val) {
		constexpr C object{};
		return size_t(&(object.*val)) - size_t(&object);
	}
	template <typename C, typename T>
	inline size_t constexpr size_of(T val) {
		constexpr C object{};
		return sizeof(object.*val);
	}
	template <typename C, typename T>
	inline std::string_view constexpr name_of(T val) {
		constexpr C object{};
		return typeid(object.*val).name();
	}
	class MetaData {
	public:
		MetaData(char* data, size_t size) : m_data(data), m_size(size) {};
		char* getData() {
			return m_data;
		}
		size_t getSize() {
			return m_size;
		}
	private:
		char* m_data;
		size_t m_size;
	};
	template <typename C>
	class _MetaProperty {
	public:
		virtual size_t getByteSize() = 0;
		virtual size_t getSize(C data) = 0;
		virtual size_t getOffset() = 0;
		virtual std::string_view getName() = 0;
		virtual std::string_view getData(C data) = 0;
		virtual void loadFromData(C* value, std::string_view name, MetaData& data) = 0;
	};
	template <typename C, typename T>
	class MetaProperty : public _MetaProperty<C> {
	public:
		MetaProperty(T member) : m_member(member) {};
		size_t getByteSize() {
			return size_of<C, T>(m_member);
		}
		size_t getSize(C data) {
			return size_of<C, T>(m_member);
		}
		size_t getOffset() {
			return offset_of<C, T>(m_member);
		}
		std::string_view getName() {
			return name_of<C, T>(m_member);
		}
		std::string_view getData(C data) {
			return std::string_view((char*)(&(data.*m_member)), getSize(data));
		}
		void loadFromData(C* value, std::string_view name, MetaData& data) {
			Buffer<char> buf(data.getData(), data.getSize());
			size_t size;
			buf.readData(&size, sizeof(size_t));
			char* className = buf.readString();
			if (!compareString(className, typeid(C).name())) {
				WARN("Value isn't of same type as provided MetaData!");
			};
			while (buf.getIndex() < data.getSize()) {
				char* typeName = buf.readString();
				char* dataName = buf.readString();
				size_t skipSize;
				buf.readData(&skipSize, sizeof(size_t));
				if (compareString(name.data(), dataName)) {
					if (!compareString(typeName, getName().data())) {
						WARN("Meta data contains different element types!");
					}
					size_t byteSize;
					buf.readData(&byteSize, sizeof(size_t));
					if (byteSize != getByteSize()) {
						ERROR("Size of saved MetaData isn't correct!");
						return;
					}
					memcpy((char*)value + getOffset(), buf.getCurrentBuffer(), byteSize);
					return;
				}
				buf.skip(skipSize);
			}
		}
	private:
		T m_member;
	};
	template <typename C>
	class MetaProperty <C, const char* C::*> : public _MetaProperty<C> {
	public:
		MetaProperty(const char* C::* member) : m_member(member) {};
		size_t getByteSize() {
			return sizeof(const char*);
		}
		size_t getSize(C data) {
			return strlen(data.*m_member);
		}
		size_t getOffset() {
			return offset_of<C, const char* C::*>(m_member);
		}
		std::string_view getName() {
			return name_of<C, const char* C::*>(m_member);
		}
		std::string_view getData(C data) {
			return std::string_view(data.*m_member, getSize(data));
		}
		void loadFromData(C* value, std::string_view name, MetaData& data) {
			Buffer<char> buf(data.getData(), data.getSize());
			size_t size;
			buf.readData(&size, sizeof(size_t));
			char* className = buf.readString();
			if (!compareString(className, typeid(C).name())) {
				WARN("Value isn't of same type as provided MetaData!");
			};
			while (buf.getIndex() < data.getSize()) {
				char* typeName = buf.readString();
				char* dataName = buf.readString();
				size_t skipSize;
				buf.readData(&skipSize, sizeof(size_t));
				if (compareString(name.data(), dataName)) {
					if (!compareString(typeName, getName().data())) {
						WARN("Meta data contains different element types!");
					}
					size_t byteSize;
					buf.readData(&byteSize, sizeof(size_t));
					if (byteSize != getByteSize()) {
						ERROR("Size of saved MetaData isn't correct!");
						return;
					}
					char* ptr = (char*)(buf.getCurrentBuffer());
					memcpy((char*)value + getOffset(), &ptr, sizeof(const char*));
					return;
				}
				buf.skip(skipSize);
			}
		}
	private:
		const char* C::* m_member;
	};
	template <typename C>
	class MetaType {
	public:
		MetaType(std::string_view name)
			: m_name(name) {};
		template <typename T>
		MetaType& property(std::string_view name, T value) {
			MetaProperty<C, T>* prop = new MetaProperty<C, T>(value);
			m_properties.emplace(name, prop);
			return *this;
		}
		MetaData getData(C value) {
			size_t size = m_name.size() + 1;
			size += sizeof(size_t); // size of data 
			size_t* sizes = new size_t[m_properties.size()]{ 0 };
			size_t sizeIndex = 0;
			for (auto& prop : m_properties) {
				size_t propSize = 0;
				size += prop.second->getName().size() + 1; // type name to validate
				size += prop.first.size() + 1; // member name
				size += sizeof(size_t); // skip size
				propSize += sizeof(size_t); // type size
				propSize += prop.second->getData(value).size() + 1;
				size += propSize;
				sizes[sizeIndex] = propSize;
				sizeIndex++;
			}
			Buffer<char> buf(size);
			sizeIndex = 0;
			buf.writeData(&size, sizeof(size_t));
			buf.writeData(m_name.data(), m_name.size() + 1);
			for (auto& prop : m_properties) {
				std::string_view typeName = prop.second->getName();
				std::string_view propertyName = prop.first;
				size_t typeSize = prop.second->getByteSize();
				std::string_view data = prop.second->getData(value);
				buf.writeData(typeName.data(), typeName.size() + 1);
				buf.writeData(propertyName.data(), propertyName.size() + 1);
				buf.writeData(&sizes[sizeIndex], sizeof(size_t));
				buf.writeData(&typeSize, sizeof(size_t));
				buf.writeData(data.data(), data.size());
				buf.skip();
				sizeIndex++;
			}
			return MetaData(buf.getBuffer(), size);
		}
		void loadFromData(C* value, MetaData& data) {
			for (auto& prop : m_properties) {
				prop.second->loadFromData(value, prop.first, data);
			}
		}
	private:
		std::string_view m_name;
		std::unordered_map<std::string_view, _MetaProperty<C>*> m_properties;
	};
	template <typename C>
	MetaType<C>* registeredType;
	template <typename C>
	constexpr MetaType<C>& registerType() {
		std::string_view typeName = typeid(C).name();
		if (registeredType<C> != nullptr) {
			WARN("Class was already registered!");
			delete registeredType<C>;
		}
		registeredType<C> = new MetaType<C>{ typeName };
		INFO("Successfully registered \"{}\" type.", typeName);
		return *registeredType<C>;
	}
	std::unordered_map<std::string_view, MetaData> s_metaDatas;
	template <typename C>
	void createMetaData(C value, const char* name) {
		std::string_view typeName = typeid(C).name();
		if (registeredType<C> == nullptr) {
			ERROR("Type \"{}\" isn't registered!", typeName);
			return;
		}
		if (s_metaDatas.contains(name)) {
			s_metaDatas.at(name) = registeredType<C>->getData(value);
			return;
		}
		s_metaDatas.emplace(name, registeredType<C>->getData(value));
	}
	void saveMetaData(const char* output) {
		FILE* f;
		fopen_s(&f, output, "wb");
		if (f == nullptr) {
			ERROR("Failed to open output file \"{}\"!", output);
			return;
		}
		size_t amount = s_metaDatas.size();
		size_t size = 4 + sizeof(size_t);
		for (auto& data : s_metaDatas)
			size += data.first.size() + 1 + data.second.getSize();
		Buffer<char> buf(size);
		buf.writeData("IRIS", 4);
		buf.writeData(&amount, sizeof(size_t));
		for (auto& data : s_metaDatas) {
			buf.writeData(data.first.data(), data.first.size());
			buf.skip();
			buf.writeData(data.second.getData(), data.second.getSize());
		}
		fwrite(buf.getBuffer(), buf.getSize(), 1, f);
		u32 crc = CRC32(buf.getBuffer(), buf.getSize());
		fwrite(&crc, sizeof(u32), 1, f);
		fclose(f);
		INFO("Saved data to \"{}\".", output);
	}
	void loadMetaData(const char* input) {
		FILE* f;
		fopen_s(&f, input, "rb");
		if (f == nullptr) {
			ERROR("Failed to open input file \"{}\"!", input);
			return;
		}
		size_t size = getFileSize(f) - sizeof(u32);
		Buffer<char> buf(size);
		buf.writeDataFromFile(f, size);
		u32 savedCRC;
		fread(&savedCRC, sizeof(u32), 1, f);
		fclose(f);
		u32 trueCRC = CRC32(buf.getBuffer(), buf.getSize());
		buf.resetIndex();
		if (!buf.compare("IRIS", 4)) {
			ERROR("File \"{}\" isn't a valid Iris serializer file!", input);
			return;
		}
		if (savedCRC != trueCRC) {
			ERROR("File \"{}\" CRC mismatch!", input);
			return;
		}
		size_t amount;
		buf.readData(&amount, sizeof(size_t));
		for (size_t i = 0; i < amount; i++) {
			char* name = buf.readString();
			size_t blockSize;
			buf.readData(&blockSize, sizeof(size_t), false);
			if (blockSize > size) {
				ERROR("MetaData file \"{}\" is corrupted!", input);
				return;
			}
			INFO("Found MetaData \"{}\" with size of {} bytes.", name, blockSize);
			if (s_metaDatas.contains(name)) {
				WARN("File contains multiple datas with same identificator (\"{}\")!", name);
			}
			s_metaDatas.emplace(name, MetaData{ buf.getCurrentBuffer(), blockSize });
			buf.skip(blockSize);
		}
	}
	template <typename C>
	void loadFromMetaData(C& value, const char* name) {
		if (!s_metaDatas.contains(name)) {
			ERROR("Loaded MetaData doesn't contain data for \"{}\" variable!", name);
			return;
		}
		INFO("Loading in values using identifier \"{}\".", name);
		registeredType<C>->loadFromData(&value, s_metaDatas.at(name));
	}
}