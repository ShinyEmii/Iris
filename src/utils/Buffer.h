#pragma once
#include "../debugger/Debugger.h"
namespace Iris {
	namespace Utility {
		template <typename T>
		class Buffer {
		public:
			Buffer(T* ptr, size_t size) : m_ptr(ptr), m_size(size), m_index(0) {};
			Buffer(size_t size) : m_ptr(new T[size]{0}), m_size(size), m_index(0) {};
			size_t getSize() {
				return m_size;
			}
			T* getBuffer() {
				return m_ptr;
			}
			T* getCurrentBuffer() {
				return m_ptr + m_index;
			}
			void readData(void* data, size_t size, bool moveIndex = true) {
				if (m_index + size > m_size) {
					ERROR("Buffer overflow!");
					return;
				}
				memcpy(data, m_ptr + m_index, size);
				if (moveIndex) m_index += size;
			}
			char* readString(bool moveIndex = true) {
				char* str = m_ptr + m_index;
				if (moveIndex) m_index += strlen(str) + 1;
				return str;
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
			void skip(size_t size = 1) {
				m_index += size;
			}
			void resetIndex() {
				m_index = 0;
			}
			bool compare(const T* data, size_t size, bool moveIndex = true) {
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
			T* m_ptr;
			size_t m_size;
		};
	}
}