#pragma once
#include "../utils/utility.h"
#include "../utils/Buffer.h"
#include "../debugger/Debugger.h"
#include "../utils/CRC32.h"
/* PACKAGE
4 bytes - IRIS tag
8 bytes - size_t / amount of assets inside of package
ASSET HEADER
8 bytes - size_t / length of asset data
varying - string / name of asset including null byte
varying - bytes  / raw data of asset
1 byte  - null   / padding
4 bytes - u32    / CRC32
1 byte  - null   / padding
*/
namespace Iris {
	namespace Assets {
		struct Asset {
		public:
			Asset()
				: size(0), data(nullptr) {};
			Asset(size_t size, char* data)
				: size(size), data(data) {};
			size_t size;
			char* data;
		};
		class Package {
		public:
			Package(const char* src)
				: m_src(src), m_size(0), m_data(nullptr) {
				FILE* file;
				fopen_s(&file, src, "rb");
				if (file == nullptr) {
					ERROR("File \"{}\" doesn't exist!", src);
					return;
				}
				m_size = Utility::getFileSize(file);
				m_data = new char[m_size] {0};
				Utility::Buffer buf(m_data, m_size);
				buf.writeDataFromFile(file, m_size, false);
				fclose(file);
				if (!buf.compare("IRIS", 4)) {
					ERROR("File \"{}\" isn't a valid Iris package file!", src);
					return;
				}
				size_t assetCount;
				buf.readData(&assetCount, sizeof(size_t));
				for (size_t i = 0; i < assetCount; i++) {
					size_t assetLength;
					buf.readData(&assetLength, sizeof(size_t));
					if (assetLength > m_size) {
						ERROR("Package data is corrupted!");
						return;
					}
					char* name = buf.readString();
					char* data = buf.getCurrentBuffer();
					u32 trueCRC = Utility::CRC32(data, assetLength);
					u32 savedCRC;
					buf.skip(assetLength + 1);
					buf.readData(&savedCRC, sizeof(u32));
					buf.skip(1);
					if (trueCRC != savedCRC) {
						ERROR("Asset \"{}\" CRC mismatch!", name);
						continue;
					}
					m_assets.emplace(name, Asset(assetLength, data));
				}
				INFO("Loaded package \"{}\" containing {} assets.", src, assetCount);
			}
			Asset getAsset(std::string_view name) {
				if (!m_assets.contains(name)) {
					ERROR("Package \"{}\" doesn't contain asset of name \"{}\"!", m_src, name);
					return Asset();
				}
				return m_assets.at(name);
			}
			const char* getName() {
				return m_src;
			}
		private:
			std::unordered_map<std::string_view, Asset> m_assets;
			const char* m_src;
			size_t m_size;
			char* m_data;
		};
		Package loadPackage(const char* src) {
			return Package(src);
		}
		void createPackage(const char* output, std::vector<const char*> args) {
			size_t bufSize = 4 + sizeof(size_t);
			size_t* sizes = new size_t[args.size()]{ 0 };
			size_t amount = 0;
			for (size_t i = 0; i < args.size(); i++) {
				FILE* f;
				fopen_s(&f, args[i], "rb");
				if (f == nullptr) {
					ERROR("File \"{}\" doesn't exist!", args[i]);
					continue;
				}
				amount++;
				bufSize += sizeof(size_t) + strlen(args[i]) + Utility::getFileSize(f) + 3 + sizeof(u32);
				sizes[i] = Utility::getFileSize(f);
				fclose(f);
			}
			size_t index = 4 + sizeof(size_t);
			Utility::Buffer<char> buf(bufSize);
			buf.writeData("IRIS", 4);
			buf.writeData(&amount, sizeof(size_t));
			for (size_t i = 0; i < args.size(); i++) {
				FILE* f;
				fopen_s(&f, args[i], "rb");
				if (f == nullptr) {
					ERROR("File \"{}\" doesn't exist!", args[i]);
					continue;
				}
				buf.writeData(&sizes[i], sizeof(size_t));
				buf.writeData(args[i], strlen(args[i]) + 1);
				char* file = buf.getCurrentBuffer();
				buf.writeDataFromFile(f, sizes[i]);
				u32 crc = Utility::CRC32(file, sizes[i]);
				buf.skip();
				buf.writeData(&crc, sizeof(u32));
				buf.skip();
				fclose(f);
			}
			FILE* outFile;
			fopen_s(&outFile, output, "wb");
			if (outFile == nullptr) {
				ERROR("Failed to open file \"{}\"!", output);
				return;
			}
			fwrite(buf.getBuffer(), buf.getSize(), 1, outFile);
			fclose(outFile);
			INFO("Successfully created package containing {} assets.", args.size());
		}
	}
}