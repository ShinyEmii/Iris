#pragma once
#include "../utils/units.h"
#include "../debugger/Debugger.h"
#include "../renderer/Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
namespace Iris {
	namespace Scene {
		class Camera {
		public:
			Camera(glm::mat4x4 projection) : m_proj(projection), m_pos(0.0f, 0.0f) {};
			void use() {
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
			void updatePos(glm::vec2 pos) {
				m_pos = pos;
			}
		private:
			glm::vec2 m_pos;
			glm::mat4x4 m_proj;
		};
		Camera createCamera(float width, float height) {
			return Camera(glm::ortho(-width / 2.0f, width / 2.0f, -height / 2.0f, height / 2.0f, 0.1f, 100.0f));
		}
	}
}