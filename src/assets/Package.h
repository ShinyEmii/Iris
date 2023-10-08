#pragma once
#include "../utility/ByteBuffer.h"
#include "../utility/CRC32.h"
#include "Queue.h"
/* PACKAGE
4 bytes - IRIS tag
8 bytes - size_t / amount of assets inside of package
ASSET HEADER
8 bytes - size_t / length of asset data
1 byte  - enum   / asset type
varying - string / name of asset including null byte
varying - bytes  / raw data of asset
1 byte  - null   / padding
4 bytes - u32    / CRC32
1 byte  - null   / padding
*/
namespace Iris {
	namespace Loader {
		enum class AssetType {
			Shader,
			Texture,
			Sound,
			Mesh,
			Other
		};
		void loadPackage(std::string_view src) {
			Utility::ByteBuffer buf(src);
			if (buf.getSize() == 0) {
				return;
			}
			if (!buf.compare("IRIS", 4)) {
				ERROR("File \"{}\" isn't a valid Iris package file!", src);
				return;
			}
			size_t assetCount;
			buf.getValue(&assetCount);
			s_queuedResourcesCount += assetCount;
			for (size_t i = 0; i < assetCount; i++) {
				size_t assetLength;
				buf.getValue(&assetLength);
				if (assetLength > buf.getSize()) {
					ERROR("Package data is corrupted!");
					s_failedResourcesCount += assetCount;
					s_loadedResourcesCount += assetCount;
					return;
				}
				AssetType type;
				buf.getValue(&type);
				char* name = buf.getString();
				char* data = buf.getCurrentBuffer();
				uint32_t trueCRC = Utility::CRC32(data, assetLength);
				uint32_t savedCRC;
				buf.move(assetLength + 1);
				buf.getValue(&savedCRC);
				buf.move();
				if (strlen(name) == 0) {
					s_failedResourcesCount++;
					s_loadedResourcesCount++;
					continue;
				}
				if (trueCRC != savedCRC) {
					WARN("Asset \"{}\" CRC mismatch! Could be corrupted!", name);
					s_failedResourcesCount++;
					s_loadedResourcesCount++;
					continue;
				}
				if (type == AssetType::Sound) {
					Utility::ByteBuffer buf = Utility::ByteBuffer(data, assetLength);
					Audio::SoundData sound = Audio::createSoundDataFromBuffer(buf);
					if (sound.valid) {
						if (Resource::s_sounds.contains(name))
							WARN("Sound \"{}\" was already loaded before", name);
						INFO("Successfully loaded sound \"{}\"", name);
						s_loadedResourcesCount++;
						Resource::s_sounds.insert({ name, new Sound(sound) });
					} else {
						s_failedResourcesCount++;
						s_loadedResourcesCount++;
					}
				}
				if (type == AssetType::Texture) {
					Utility::ByteBuffer buf(data, assetLength);
					TextureFiltering filter;
					TextureWrapping wrap;
					buf.getValue(&filter);
					buf.getValue(&wrap);
					Utility::ByteBuffer texBuf(buf.getCurrentBuffer(), assetLength - sizeof(TextureFiltering) - sizeof(TextureWrapping));
					Renderer::TextureData tex = Renderer::createTextureDataFromBuffer(texBuf);
					tex.filter = filter;
					tex.wrap = wrap;
					if (tex.valid) {
						if (Resource::s_textures.contains(name))
							WARN("Sound \"{}\" was already loaded before", name);
						INFO("Successfully loaded texture \"{}\"", name);
						s_loadedResourcesCount++;
						Resource::s_textures.at(name).setAsset(new Texture(tex));
					}
					else {
						Resource::s_textures.at(name).setStatus(AssetStatus::Invalid);
						s_failedResourcesCount++;
						s_loadedResourcesCount++;
					}
				}
			}
			INFO("Loaded package \"{}\" containing {} assets.", src, assetCount);
		}
		void createPackage(std::string_view output) {
			size_t bufSize = 4 + sizeof(size_t);
			size_t* sizes = new size_t[Resource::s_sounds.size()]{ 0 };
			size_t sizeIndex = 0;
			size_t amount = 0;
			for (auto& sound : Resource::s_sounds) {
				if (sound.second.getStatus() != AssetStatus::Valid)
					continue;
				FILE* f;
				fopen_s(&f, sound.first.data(), "rb");
				if (f == nullptr) {
					ERROR("File \"{}\" doesn't exist!", sound.first);
					continue;
				}
				amount++;
				fseek(f, 0, SEEK_END);
				size_t fileSize = ftell(f);
				fclose(f);
				bufSize += sizeof(size_t) + sizeof(AssetType) + strlen(sound.first.data()) + fileSize + 3 + sizeof(uint32_t);
				sizes[sizeIndex] = fileSize;
				sizeIndex++;
			}
			for (auto& tex : Resource::s_textures) {
				if (tex.second.getStatus() != AssetStatus::Valid)
					continue;
				FILE* f;
				fopen_s(&f, tex.first.data(), "rb");
				if (f == nullptr) {
					ERROR("File \"{}\" doesn't exist!", tex.first);
					continue;
				}
				amount++;
				fseek(f, 0, SEEK_END);
				size_t fileSize = ftell(f);
				fclose(f);
				bufSize += sizeof(size_t) + sizeof(AssetType) + strlen(tex.first.data()) + sizeof(TextureFiltering) + sizeof(TextureWrapping) + fileSize + 3 + sizeof(uint32_t);
				sizes[sizeIndex] = fileSize;
				sizeIndex++;
			}
			Utility::ByteBuffer buf(bufSize);
			buf.writeData("IRIS", 4);
			buf.writeData(&amount, sizeof(size_t));
			sizeIndex = 0;
			for (auto& sound : Resource::s_sounds) {
				if (sound.second.getStatus() != AssetStatus::Valid)
					continue;
				FILE* f;
				fopen_s(&f, sound.first.data(), "rb");
				if (f == nullptr) {
					continue;
				}
				buf.writeData(&sizes[sizeIndex], sizeof(size_t));
				buf.writeValue(AssetType::Sound);
				buf.writeData(sound.first.data(), strlen(sound.first.data()) + 1);
				char* file = buf.getCurrentBuffer();
				buf.writeDataFromFile(f, sizes[sizeIndex]);
				uint32_t crc = Utility::CRC32(file, sizes[sizeIndex]);
				sizeIndex++;
				buf.move();
				buf.writeData(&crc, sizeof(uint32_t));
				buf.move();
				fclose(f);
			}
			for (auto& tex : Resource::s_textures) {
				if (tex.second.getStatus() != AssetStatus::Valid)
					continue;
				FILE* f;
				fopen_s(&f, tex.first.data(), "rb");
				if (f == nullptr) {
					continue;
				}
				buf.writeData(&sizes[sizeIndex], sizeof(size_t));
				buf.writeValue(AssetType::Texture);
				buf.writeData(tex.first.data(), strlen(tex.first.data()) + 1);
				char* file = buf.getCurrentBuffer();
				buf.writeValue(tex.second.get().getFilter());
				buf.writeValue(tex.second.get().getWrapping());
				buf.writeDataFromFile(f, sizes[sizeIndex]);
				uint32_t crc = Utility::CRC32(file, sizes[sizeIndex] + sizeof(TextureFiltering) + sizeof(TextureWrapping));
				sizeIndex++;
				buf.move();
				buf.writeData(&crc, sizeof(uint32_t));
				buf.move();
				fclose(f);
			}
			FILE* outFile;
			fopen_s(&outFile, output.data(), "wb");
			if (outFile == nullptr) {
				ERROR("Failed to open file \"{}\"!", output);
				return;
			}
			fwrite(buf.getBuffer(), buf.getSize(), 1, outFile);
			fclose(outFile);
			INFO("Successfully created package containing {} sounds {} textures and {} shaders.", amount, 0, 0);
		}
	}
}