#pragma once
#include "utility.h"
namespace Iris {
	namespace Utility {
		static bool s_crcInit = false;
		u32 crcTable[256];
		void initCRC() {
			s_crcInit = true;
			for (u16 i = 0; i < 256; i++) {
				u32 crc = i;
				for (u16 j = 0; j < 8; j++) {
					if (crc & 1)
						crc = 0xEDB88320 ^ (crc >> 1);
					else
						crc >>= 1;
				}
				crcTable[i] = crc;
			}
		}
		u32 CRC32(const void* data, size_t length) {
			if (!s_crcInit) initCRC();
			u32 crc32 = 0xFFFFFFFFu;
			for (size_t i = 0; i < length; i++) {
				u32 lookupIndex = (crc32 ^ ((u8*)data)[i]) & 0xff;
				crc32 = (crc32 >> 8) ^ crcTable[lookupIndex];
			}
			crc32 ^= 0xFFFFFFFFu;
			return crc32;
		}
		size_t getFileSize(FILE* f) {
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
		}
	}
}