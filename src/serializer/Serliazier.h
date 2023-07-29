 #pragma once
#include <string_view>
#include "../debugger/Debugger.h"
namespace Iris {
    namespace Serializer {
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
			virtual size_t getByteSize() {
				return 0;
			}
			virtual size_t getSize(C data) {
				return 0;
			}
			virtual size_t getOffset() {
				return 0;
			}
			virtual std::string_view getName() {
				return "invalidName";
			}
			virtual std::string_view getData(C data) {
				return "invalidData";
			}
			virtual void loadFromData(C* value, std::string_view name, MetaData& data) {};
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
				size_t size;
				memcpy(&size, data.getData(), sizeof(size_t));
				char* className = data.getData() + sizeof(size_t);
				if (strncmp(className, typeid(C).name(), strlen(className)) != 0) {
					WARN("Value isn't of same type as provided MetaData!");
				}
				for (size_t i = sizeof(size_t) + strlen(className) + 1; i < data.getSize(); i) {
					char* typeName = data.getData() + i;
					i += strlen(typeName) + 1;
					char* dataName = data.getData() + i;
					i += strlen(dataName) + 1;
					size_t skipSize;
					memcpy(&skipSize, data.getData() + i, sizeof(size_t));
					i += sizeof(size_t);
					if (strncmp(name.data(), dataName, name.size()) == 0 && name.size() == strlen(dataName)) {
						if (strncmp(typeName, getName().data(), getName().size()) != 0 || strlen(typeName) != getName().size()) {
							WARN("Meta data contains different element types!");
						}
						size_t byteSize;
						memcpy(&byteSize, data.getData() + i, sizeof(size_t));
						i += sizeof(size_t);
						if (byteSize != getByteSize()) {
							ERROR("Size of saved MetaData isn't correct!");
							return;
						}
						memcpy((char*)value + getOffset(), data.getData() + i, byteSize);
						return;
					}
					i += skipSize;
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
				char* metaName = data.getData();
				size_t size;
				memcpy(&size, data.getData() + strlen(metaName) + 1, sizeof(size_t));
				char* className = data.getData() + strlen(metaName) + 1 + sizeof(size_t);
				if (strncmp(className, typeid(C).name(), strlen(className)) != 0 || strlen(className) != strlen(typeid(C).name())) {
					WARN("Value isn't of same type as provided MetaData!");
				}
				for (size_t i = strlen(metaName) + 1 + sizeof(size_t) + strlen(className) + 1; i < data.getSize(); i) {
					char* typeName = data.getData() + i;
					i += strlen(typeName) + 1;
					char* dataName = data.getData() + i;
					i += strlen(dataName) + 1;
					size_t skipSize;
					memcpy(&skipSize, data.getData() + i, sizeof(size_t));
					i += sizeof(size_t);
					if (strncmp(name.data(), dataName, name.size()) == 0 && name.size() == strlen(dataName)) {
						if (strncmp(typeName, getName().data(), getName().size()) != 0 || strlen(typeName) != getName().size()) {
							WARN("Meta data contains different element types!");
						}
						size_t byteSize;
						memcpy(&byteSize, data.getData() + i, sizeof(size_t));
						i += sizeof(size_t);
						if (byteSize != getByteSize()) {
							ERROR("Size of saved MetaData isn't correct!");
							return;
						}
						char* ptr = (char*)(data.getData() + i);
						memcpy((char*)value + getOffset(), &ptr, sizeof(const char*));
						return;
					}
					i += skipSize;
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
				m_properties.emplace( name, prop );
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
				char* buf = new char[size] {0};
				size_t index = 0;
				sizeIndex = 0;
				memcpy(buf + index, (char*)&size, sizeof(size_t));
				index += sizeof(size_t);
				memcpy(buf + index, m_name.data(), m_name.size());
				index += m_name.size() + 1;
				for (auto& prop : m_properties) {
					std::string_view typeName = prop.second->getName();
					std::string_view name = prop.first;
					size_t typeSize = prop.second->getByteSize();
					size_t offset = prop.second->getOffset();
					std::string_view data = prop.second->getData(value);
					size_t dataSize = data.size();
					memcpy(buf + index, typeName.data(), typeName.size());
					index += typeName.size() + 1;
					memcpy(buf + index, name.data(), name.size());
					index += name.size() + 1;
					memcpy(buf + index, (char*)(sizes + sizeIndex), sizeof(size_t));
					index += sizeof(size_t);
					memcpy(buf + index, (char*)&typeSize, sizeof(size_t));
					index += sizeof(size_t);
					memcpy(buf + index, data.data(), data.size());
					index += data.size() + 1;
					sizeIndex++;
				}
				return MetaData(buf, size);
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
			size_t size = 4;
			size += sizeof(size_t);
			for (auto& data : s_metaDatas) {
				size += data.first.size() + 1;
				size += data.second.getSize();
			}
			char* buf = new char[size] {0};
			size_t index = 0;
			memcpy(buf + index, "IRIS", 4);
			index += 4;
			memcpy(buf + index, (char*)&amount, sizeof(size_t));
			index += sizeof(size_t);
			for (auto& data : s_metaDatas) {
				memcpy(buf + index, data.first.data(), data.first.size());
				index += data.first.size() + 1;
				memcpy(buf + index, data.second.getData(), data.second.getSize());
				index += data.second.getSize();
			}
			fwrite(buf, size, 1, f);
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
			size_t size;
			fseek(f, 0, SEEK_END);
			size = ftell(f);
			fseek(f, 0, SEEK_SET);
			char* buf = new char[size];
			size_t index = 4;
			fread(buf, size, 1, f);
			fclose(f);
			if (strncmp(buf, "IRIS", 4) != 0) {
				ERROR("File \"{}\" isn't a valid Iris serializer file!", input);
				return;
			}
			size_t amount;
			memcpy(&amount, buf + index, sizeof(size_t));
			index += sizeof(size_t);
			for (size_t i = 0; i < amount; i++) {
				char* name = buf + index;
				index += strlen(name) + 1;
				size_t blockSize;
				memcpy(&blockSize, buf + index, sizeof(size_t));
				if (blockSize > size) {
					ERROR("MetaData file \"{}\" is corrupted!", input);
					return;
				}
				INFO("Found MetaData \"{}\" with size of {} bytes.", name, blockSize);
				if (s_metaDatas.contains(name)) {
					WARN("File contains multiple datas with same identificator (\"{}\")!", name);
				}
				s_metaDatas.emplace( name, MetaData{ buf + index, blockSize} );
				index += blockSize;
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
}