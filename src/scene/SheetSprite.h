#pragma once
#include "../renderer/Renderer.h"
#include "../debugger/Debugger.h"
#include <glm/gtc/matrix_transform.hpp>
namespace Iris {
	namespace Scene {
		class SheetSprite {
		public:
			SheetSprite(Iris::Renderer::Texture& tex, glm::vec2 spriteSize, glm::vec2 scale = { 1.0f, 1.0f }) : m_tex(&tex), m_size(spriteSize), m_scale(scale) {};
			void draw(glm::vec2 offset, glm::vec2 pos, bool flip = false) {
				if (Iris::Renderer::s_currentProgram == nullptr) {
					WARN("Tried drawing without binding any Program");
					return;
				}
				Iris::Renderer::s_currentProgram->setInt("albedo", 0);
				m_tex->bind(0);
				glm::mat4x4 model = glm::mat4x4(1.0f);
				model = glm::translate(model, glm::vec3(pos.x, pos.y, 0.0f));
				model = glm::scale(model, glm::vec3(m_scale.x, m_scale.y, 1.0f));
				Iris::Renderer::s_currentProgram->setInt("flip", flip);
				Iris::Renderer::s_currentProgram->setMatrix4x4("model", model);
				Iris::Renderer::s_currentProgram->setVec2("spriteSize", glm::vec2{ m_tex->getWidth(), m_tex->getHeight() } / m_size);
				Iris::Renderer::s_currentProgram->setVec2("spriteOffset", offset);
				if (Iris::Renderer::s_quadMesh == nullptr) {
					ERROR("Quad mesh hasn't been initialized! Didn't initialize the renderer?");
					return;
				}
				Iris::Renderer::s_quadMesh->draw();
			};
			void drawCentered(glm::vec2 offset, glm::vec2 pos, bool flip = false) {
				draw(offset, pos - m_scale / 2.0f, flip);
			}
			void setScale(glm::vec2 scale) {
				m_scale = scale;
			}
			glm::vec2& getScale() {
				return m_scale;
			}
		private:
			glm::vec2 m_size, m_scale;
			Iris::Renderer::Texture* m_tex;
		};
	}
}