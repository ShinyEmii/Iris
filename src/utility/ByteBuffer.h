#pragma once
#include "../logger/Logger.h"
namespace Iris {
	namespace Utility {
		class ByteBuffer {
		public:
			ByteBuffer(std::string_view src)
				: m_index(0), m_size(0), m_ptr(nullptr) {
				FILE* f;
				fopen_s(&f, src.data(), "rb");
				if (f == nullptr) {
					ERROR("Failed to create ByteBuffer because file \"{}\" doesn't exist!", src);
					return;
				}
				fseek(f, 0, SEEK_END);
				m_size = ftell(f);
				fseek(f, 0, SEEK_SET);
				m_ptr = new char[m_size] { 0 };
				fread(m_ptr, m_size, 1, f);
				fclose(f);
			}
			ByteBuffer(const ByteBuffer& other)
				: m_index(0), m_ptr(nullptr), m_size(0) {
				if (other.m_ptr != nullptr) {
					m_size = other.m_size;
					m_ptr = new char[m_size] { 0 };
					memcpy(m_ptr, other.m_ptr, m_size);
				}
			}
			ByteBuffer(char* ptr, size_t size)
				: m_ptr(new char[size] { 0 }), m_size(size), m_index(0) {
				memcpy(m_ptr, ptr, size);
			};
			ByteBuffer(size_t size)
				: m_ptr(new char[size] { 0 }), m_size(size), m_index(0) {};
			~ByteBuffer() {
				if (m_ptr != nullptr)
					delete m_ptr;
			}
			size_t getSize() {
				return m_size;
			}
			char* getBuffer() {
				return m_ptr;
			}
			char* getCurrentBuffer() {
				return m_ptr + m_index;
			}
			template <typename T>
			void getValue(T* ptr, std::endian endian = std::endian::little) {
				if (m_index + sizeof(T) > m_size) {
					ERROR("Buffer overflow!");
					return;
				}
				if (std::endian::native == endian)
					memcpy(ptr, m_ptr + m_index, sizeof(T));
				else
					for (size_t i = 0; i < sizeof(T); ++i)
						memcpy(ptr + sizeof(T) - i, m_ptr + m_index + i, 1);
				m_index += sizeof(T);
			}
			void getData(void* data, size_t size, bool moveIndex = true) {
				if (m_index + size > m_size) {
					ERROR("Buffer overflow!");
					return;
				}
				memcpy(data, m_ptr + m_index, size);
				if (moveIndex) m_index += size;
			}
			char* getString(bool moveIndex = true) {
				char* str = m_ptr + m_index;
				if (moveIndex) m_index += strlen(str) + 1;
				return str;
			}
			template <typename T>
			void writeValue(T data, bool moveIndex = true) {
				if (m_index + sizeof(T) > m_size) {
					ERROR("Buffer overflow!");
					return;
				}
				memcpy_s(m_ptr + m_index, m_size, &data, sizeof(T));
				if (moveIndex) m_index += sizeof(T);
			}
			void writeData(const void* data, size_t size, bool moveIndex = true) {
				if (m_index + size > m_size) {
					ERROR("Buffer overflow!");
					return;
				}
				memcpy_s(m_ptr + m_index, m_size, data, size);
				if (moveIndex) m_index += size;
			}
			void writeDataFromFile(FILE* file, size_t size, bool moveIndex = true) {
				if (m_index + size > m_size) {
					ERROR("Buffer overflow!");
					return;
				}
				if (file == nullptr) {
					ERROR("Given file is invalid!");
					return;
				}
				fread_s(m_ptr + m_index, m_size, size, 1, file);
				if (moveIndex) m_index += size;
			}
			size_t getIndex() {
				return m_index;
			}
			void move(size_t size = 1) {
				m_index += size;
			}
			void resetIndex() {
				m_index = 0;
			}
			bool compare(const char* data, size_t size, bool moveIndex = true) {
				if (m_index + size > m_size) {
					ERROR("Buffer overflow!");
					return false;
				}
				for (size_t i = 0; i < size; i++) {
					if (m_ptr[m_index + i] != data[i]) {
						if (moveIndex) m_index += size;
						return false;
					}
				}
				if (moveIndex) m_index += size;
				return true;
			}
		private:
			size_t m_index;
			char* m_ptr;
			size_t m_size;
		};
	}
}