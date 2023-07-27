#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "../../vendors/stb_image.h"
#include "../debugger/Debugger.h"
#include "../assets/Package.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
namespace Iris {
	namespace Renderer {
		enum class TextureFiltering {
			LINEAR,
			NEAREST
		};
		enum class TextureWrapping {
			REPEAT,
			MIRRORED,
			CLAMP
		};
		class Texture {
		public:
			Texture()
				: m_width(2), m_height(2), m_depth(4), m_filter(TextureFiltering::NEAREST), m_wrap(TextureWrapping::CLAMP) {
				unsigned char missingTexture[16] = { 255, 0, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255, 255, 0, 255, 255 };
				glGenTextures(1, &m_tex);
				glBindTexture(GL_TEXTURE_2D, m_tex);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, missingTexture);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			Texture(char* data, size_t size, TextureFiltering filter, TextureWrapping wrap, u8 channels, bool free = true)
			: m_filter(filter), m_wrap(wrap) {
				unsigned char* stbiData = stbi_load_from_memory((unsigned char*)data, (int)size, (int*)&m_width, (int*)&m_height, (int*)&m_depth, channels);
				glGenTextures(1, &m_tex);
				glBindTexture(GL_TEXTURE_2D, m_tex);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				switch (m_depth) {
				case 1:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, stbiData);
					break;
				case 2:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, m_width, m_height, 0, GL_RG, GL_UNSIGNED_BYTE, stbiData);
					break;
				case 3:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, stbiData);
					break;
				case 4:
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, stbiData);
					break;
				default:
					ERROR("Invalid amount of channels! Texture's corrupted?");
					return;
					break;
				}
				switch (m_wrap) {
				default:
				case TextureWrapping::REPEAT:
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					break;
				case TextureWrapping::MIRRORED:
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
					break;
				case TextureWrapping::CLAMP:
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					break;
				}
				switch (m_filter) {
				default:
				case TextureFiltering::LINEAR:
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					break;
				case TextureFiltering::NEAREST:
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					break;
				}
				glGenerateMipmap(GL_TEXTURE_2D);
				delete[] stbiData;
				if (free) delete[] data;
				INFO("Texture loaded.");
			}
			~Texture() {
				glDeleteTextures(1, &m_tex);
			}
			void bind(u8 id) {
				glActiveTexture(GL_TEXTURE0 + id);
				glBindTexture(GL_TEXTURE_2D, m_tex);
			}
			GLuint getTexture() {
				return m_tex;
			}
			i32 getWidth() {
				return m_width;
			}
			i32 getHeight() {
				return m_height;
			}
			i32 getDepth() {
				return m_depth;
			}
			void setWrapping(TextureWrapping wrap) {
				glBindTexture(GL_TEXTURE_2D, m_tex);
				m_wrap = wrap;
				switch (m_wrap) {
				default:
				case TextureWrapping::REPEAT:
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					break;
				case TextureWrapping::MIRRORED:
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
					break;
				case TextureWrapping::CLAMP:
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					break;
				}
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			void setFiltering(TextureFiltering filter) {
				glBindTexture(GL_TEXTURE_2D, m_tex);
				m_filter = filter;
				switch (m_filter) {
				default:
				case TextureFiltering::LINEAR:
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					break;
				case TextureFiltering::NEAREST:
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					break;
				}
				glGenerateMipmap(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			TextureWrapping getWrapping() {
				return m_wrap;
			}
			TextureFiltering getFilter() {
				return m_filter;
			}
		private:
			GLuint m_tex;
			i32 m_width, m_height, m_depth;
			TextureWrapping m_wrap;
			TextureFiltering m_filter;
		};
		Texture loadTexture(const char* src, TextureFiltering filter = TextureFiltering::LINEAR, TextureWrapping wrap = TextureWrapping::REPEAT, u8 channels = 0) {
			INFO("Loading Texture from file \"{}\".", src);
			FILE* f;
			fopen_s(&f, src, "rb");
			if (f == nullptr) {
				ERROR("Failed to load sound from file {}!", src);
				return Texture();
			}
			size_t size;
			fseek(f, 0, SEEK_END);
			size = ftell(f);
			fseek(f, 0, SEEK_SET);
			char* data = new char[size] {0};
			fread(data, size, 1, f);
			fclose(f);
			return Texture(data, size, filter, wrap, channels);
		}
		Texture loadTexture(Assets::Package& package, const char* name, TextureFiltering filter = TextureFiltering::LINEAR, TextureWrapping wrap = TextureWrapping::REPEAT, u8 channels = 0) {
			INFO("Loading Texture from package \"{}\" with name \"{}\".", package.getName(), name);
			Assets::Asset texture = package.getAsset(name);
			if (texture.size == 0) return Texture();
			return Texture(texture.data, texture.size, filter, wrap, channels, false);
		}
	}
}