#pragma once
#include "../utils/utility.h"
#include "../debugger/Debugger.h"
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
				fseek(file, 0, SEEK_END);
				m_size = ftell(file);
				fseek(file, 0, SEEK_SET);
				m_data = new char[m_size] {0};
				fread_s(m_data, m_size, m_size, 1, file);
				fclose(file);
				if (strncmp(m_data, "IRIS", 4) != 0) {
					ERROR("File \"{}\" isn't a valid Iris package file!", src);
					return;
				}
				u32 assetCount;
				memcpy(&assetCount, m_data + 4, sizeof(u32));
				size_t index = 8;
				for (u32 i = 0; i < assetCount; i++) {
					u32 assetLength;
					memcpy(&assetLength, m_data + index, sizeof(u32));
					index += sizeof(u32);
					char* name = m_data + index;
					size_t nameLength = strlen(name) + 1;
					index += nameLength;
					index += 1; // padding
					m_assets.emplace(name, Asset(assetLength, m_data + index));
					index += assetLength;
					index += 1; // padding
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
			size_t bufSize = 4 + sizeof(u32);
			u32* sizes = new u32[args.size()]{ 0 };
			for (size_t i = 0; i < args.size(); i++) {
				bufSize += strlen(args[i]) + 1;
				bufSize += sizeof(u32);
				FILE* f;
				fopen_s(&f, args[i], "rb");
				if (f == nullptr) {
					ERROR("File \"{}\" doesn't exist!", args[i]);
					continue;
				}
				fseek(f, 0, SEEK_END);
				bufSize += ftell(f);
				bufSize += 2; // padding
				sizes[i] = ftell(f);
				fclose(f);
			}
			size_t index = 4 + sizeof(u32);
			u32 size = (u32)args.size();
			char* buf = new char[bufSize] {0};
			memcpy_s(buf, bufSize, "IRIS", 4);
			memcpy_s(buf + 4, bufSize - 4, &size, sizeof(u32));
			for (size_t i = 0; i < args.size(); i++) {
				memcpy_s(buf + index, bufSize - index, &sizes[i], sizeof(u32));
				index += sizeof(u32);
				memcpy_s(buf + index, bufSize - index, args[i], strlen(args[i]) + 1);
				index += strlen(args[i]) + 1;
				index += 1; // padding
				FILE* f;
				fopen_s(&f, args[i], "rb");
				if (f == nullptr) {
					ERROR("File \"{}\" doesn't exist!", args[i]);
					continue;
				}
				fread_s(buf + index, bufSize - index, sizes[i], 1, f);
				index += sizes[i];
				index += 1; // padding
				fclose(f);
			}
			FILE* outFile;
			fopen_s(&outFile, output, "wb");
			if (outFile == nullptr) {
				ERROR("Failed to open file \"{}\"!", output);
				return;
			}
			fwrite(buf, bufSize, 1, outFile);
			fclose(outFile);
			INFO("Successfully created package containing {} assets.", args.size());
		}
	}
}