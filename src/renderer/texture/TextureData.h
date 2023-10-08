#pragma once
#include "../../logger/Logger.h"
#include "../../utility/ByteBuffer.h"
namespace Iris {
	enum class TextureFiltering {
		LINEAR,
		NEAREST
	};
	enum class TextureWrapping {
		REPEAT,
		MIRRORED,
		CLAMP
	};
	namespace Renderer {
		struct TextureData {
			bool valid;
			std::string data;
			int16_t width, height;
			int8_t depth;
			TextureFiltering filter;
			TextureWrapping wrap;
		};
		TextureData createTextureDataFromBuffer(Utility::ByteBuffer& buf) {
			int16_t width, height;
			int8_t depth;
			char* stbi_data = (char*)stbi_load_from_memory((unsigned char*)buf.getBuffer(), (int)buf.getSize(), (int*)&width, (int*)&height, (int*)&depth, 0);
			if (stbi_data == nullptr) return {};
			std::string data = std::string(stbi_data);
			delete stbi_data;
			return TextureData{ true, data, width, height, depth, TextureFiltering::LINEAR, TextureWrapping::REPEAT };
		}
	}
}