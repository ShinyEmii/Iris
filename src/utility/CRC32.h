#pragma once
namespace Iris {
	namespace Utility {
		static bool s_crcInit = false;
		uint32_t crcTable[256];
		void init() {
			s_crcInit = true;
			for (uint16_t i = 0; i < 256; i++) {
				uint32_t crc = i;
				for (uint16_t j = 0; j < 8; j++) {
					if (crc & 1)
						crc = 0xEDB88320 ^ (crc >> 1);
					else
						crc >>= 1;
				}
				crcTable[i] = crc;
			}
		}
		uint32_t CRC32(const void* data, size_t length) {
			if (!s_crcInit) init();
			uint32_t crc32 = 0xFFFFFFFFu;
			for (size_t i = 0; i < length; i++) {
				uint32_t lookupIndex = (crc32 ^ ((uint8_t*)data)[i]) & 0xff;
				crc32 = (crc32 >> 8) ^ crcTable[lookupIndex];
			}
			crc32 ^= 0xFFFFFFFFu;
			return crc32;
		}
		/*size_t getFileSize(FILE* f) {
			if (f == nullptr) {
				ERROR("Given file is invalid!");
				return 0;
			}
			long cur = ftell(f);
			fseek(f, 0, SEEK_END);
			size_t size = ftell(f);
			fseek(f, cur, SEEK_SET);
			return size;
		}
		bool compareString(const char* a, const char* b) {
			if (strlen(a) != strlen(b)) return false;
			if (strncmp(a, b, strlen(a)) != 0) return false;
			return true;
		}*/
	}
}