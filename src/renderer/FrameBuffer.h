#pragma once
#include "../debugger/Debugger.h"
#include "../utils/units.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
namespace Iris {
	namespace Renderer {
		class Framebuffer {
		public:
			Framebuffer(u16 width, u16 height) {
				glGenFramebuffers(1, &m_fbo);
				glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

				glGenTextures(1, &m_texture);
				glBindTexture(GL_TEXTURE_2D, m_texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

				glGenRenderbuffers(1, &m_rbo);
				glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
					ERROR("Framebuffer incomplete!");
					return;
				}
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glBindTexture(GL_TEXTURE_2D, 0);
				glBindRenderbuffer(GL_RENDERBUFFER, 0);
				INFO("FrameBuffer successfully created!");
			};
			~Framebuffer() {
				glDeleteFramebuffers(1, &m_fbo);
				glDeleteTextures(1, &m_texture);
				glDeleteRenderbuffers(1, &m_rbo);
			};
			u32 getTexture() {
				return m_texture;
			};
			void rescale(u16 width, u16 height) {
				glBindTexture(GL_TEXTURE_2D, m_texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

				glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
			};
			void bind() {
				glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
			};
			void unbind() {
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			};
		private:
			GLuint m_fbo, m_texture, m_rbo;
		};
	}
}