#pragma once
#include "../utils/units.h"
#include "../renderer/Renderer.h"
#include "../debugger/Debugger.h"

#include <glm/gtc/matrix_transform.hpp>
namespace Iris {
	namespace Scene {
		class Sprite {
		public:
			Sprite(Iris::Renderer::Texture& tex, glm::vec2 scale = { 1.0f, 1.0f }, glm::vec2 anchorPoint = { 0.5f, 0.5f }) : m_tex(&tex), m_scale(scale), m_rotation(0.0f), m_anchorPoint(anchorPoint) {};
			void draw(glm::vec2 pos, bool flip = false) {
				if (Iris::Renderer::s_currentProgram == nullptr) {
					WARN("Tried drawing without using any Program");
					return;
				}
				Iris::Renderer::s_currentProgram->setInt("albedo", 0);
				m_tex->bind(0);
				glm::mat4x4 trans = glm::translate(glm::mat4x4(1.0f), glm::vec3(pos.x, pos.y, 0.0f));
				glm::mat4x4 scale = glm::scale(glm::mat4x4(1.0f), glm::vec3(m_scale.x, m_scale.y, 1.0f));
				glm::mat4x4 rotation = glm::rotate(glm::mat4x4(1.0f), glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
				glm::mat4x4 rotationAnchorOffset = glm::translate(glm::mat4x4(1.0f), glm::vec3(-m_anchorPoint.x, -m_anchorPoint.y, 0.0));
				glm::mat4x4 restoreOffset = glm::translate(glm::mat4x4(1.0f), glm::vec3(m_anchorPoint.x, m_anchorPoint.y, 0.0));
				glm::mat4x4 model = trans * scale * restoreOffset * rotation * rotationAnchorOffset * glm::mat4x4(1.0f);
				Iris::Renderer::s_currentProgram->setInt("flip", flip);
				Iris::Renderer::s_currentProgram->setMatrix4x4("model", model);
				if (Iris::Renderer::s_quad == nullptr) {
					ERROR("Quad mesh hasn't been initialized!");
					return;
				}
				Iris::Renderer::s_quad->draw();
			};
			void setScale(glm::vec2 scale) {
				m_scale = scale;
			}
			glm::vec2& getScale() {
				return m_scale;
			}
			void setRotation(f32 rotation) {
				m_rotation = rotation;
			}
			f32& getRotation() {
				return m_rotation;
			}
		private:
			f32 m_rotation;
			glm::vec2 m_scale;
			glm::vec2 m_anchorPoint;
			Iris::Renderer::Texture* m_tex;
		};
	}
}