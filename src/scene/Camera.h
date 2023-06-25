#pragma once
#include "../utils/units.h"
#include "../debugger/Debugger.h"
#include "../renderer/Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
namespace Iris {
	namespace Scene {
		class Camera {
		public:
			Camera(f32 width, f32 height) : m_width(width), m_height(height), m_pos(0.0f, 0.0f) {
				m_proj = glm::ortho(0.0f, m_width, m_height, 0.0f, 0.1f, 100.0f);
			}
			void use() {
				if (Iris::Renderer::s_currentProgram == nullptr) {
					ERROR("Tried using camera without using Program");
					return;
				}
				Iris::Renderer::s_currentProgram->setMatrix4x4("proj", getProjectionMatrix());
				Iris::Renderer::s_currentProgram->setMatrix4x4("view", getViewMatrix());
			}
			glm::mat4x4& getProjectionMatrix() {
				return m_proj;
			}
			void updateProjectionMatrix(glm::mat4x4 projection) {
				m_proj = projection;
			}
			glm::mat4x4 getViewMatrix() {
				return glm::lookAt(glm::vec3(m_pos.x, m_pos.y, 100.0f), glm::vec3(m_pos.x, m_pos.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			glm::vec2& getPos() {
				return m_pos;
			}
			f32 getWidth() {
				return m_width;
			}
			f32 getHeight() {
				return m_height;
			}
			void updatePos(glm::vec2 pos) {
				m_pos = pos;
			}
		private:
			f32 m_width, m_height;
			glm::vec2 m_pos;
			glm::mat4x4 m_proj;
		};
	}
}