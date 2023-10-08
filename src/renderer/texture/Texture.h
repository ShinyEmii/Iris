#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "../../../vendors/stb_image.h"
#include "TextureData.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
namespace Iris {
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
		};
		Texture(Renderer::TextureData tex)
			: m_filter(tex.filter), m_wrap(tex.wrap), m_depth(tex.depth), m_width(tex.width), m_height(tex.height) {
			glGenTextures(1, &m_tex);

			glBindTexture(GL_TEXTURE_2D, m_tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			switch (m_depth) {
			case 1:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, tex.data.data());
				break;
			case 2:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, m_width, m_height, 0, GL_RG, GL_UNSIGNED_BYTE, tex.data.data());
				break;
			case 3:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex.data.data());
				break;
			case 4:
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.data.data());
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
		}
		~Texture() {
			glDeleteTextures(1, &m_tex);
		}
		void bindTexture(uint8_t id) {
			glActiveTexture(GL_TEXTURE0 + id);
			glBindTexture(GL_TEXTURE_2D, m_tex);
		}
		GLuint getTexture() {
			return m_tex;
		}
		int16_t getWidth() {
			return m_width;
		}
		int16_t getHeight() {
			return m_height;
		}
		int8_t getDepth() {
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
		int16_t m_width, m_height;
		int8_t m_depth;
		TextureWrapping m_wrap;
		TextureFiltering m_filter;
	};
}