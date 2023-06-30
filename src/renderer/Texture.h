#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "../../vendors/stb_image.h"
#include "../debugger/Debugger.h"
#include "../utils/units.h"
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
			Texture(const char* path, TextureFiltering filter = TextureFiltering::LINEAR, TextureWrapping wrap = TextureWrapping::CLAMP, u8 channels = 0)
			: m_filter(filter), m_wrap(wrap) {
				unsigned char* stbiData = stbi_load(path, (int*)&m_width, (int*)&m_height, (int*)&m_depth, channels);
				if (stbiData == nullptr) {
					ERROR("Failed to load texture ( \"{}\" ). Invalid path? Using default checkerboard texture.", path);
					unsigned char missingTexture[16] = { 255, 0, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255, 255, 0, 255, 255 };
					stbiData = new unsigned char[16];
					memcpy(stbiData, missingTexture, 16);
					m_width = 2;
					m_height = 2;
					m_depth = 4;
				}
				glGenTextures(1, &m_texture);
				glBindTexture(GL_TEXTURE_2D, m_texture);
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
					ERROR("Invalid amount of channels! Texture corrupted?");
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
				INFO("Texture ( \"{}\" ) loaded!", path);
			};
			~Texture() {
				glDeleteTextures(1, &m_texture);
			}
			void bind(u8 id) {
				glActiveTexture(GL_TEXTURE0 + id);
				glBindTexture(GL_TEXTURE_2D, m_texture);
			}
			GLuint getTexture() {
				return m_texture;
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
				glBindTexture(GL_TEXTURE_2D, m_texture);
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
				glBindTexture(GL_TEXTURE_2D, m_texture);
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
			GLuint m_texture;
			i32 m_width, m_height, m_depth;
			TextureWrapping m_wrap;
			TextureFiltering m_filter;
		};
	}
}