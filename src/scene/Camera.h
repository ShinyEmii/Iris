#pragma once
#include "../debugger/Debugger.h"
#include "../renderer/Renderer.h"
#include "Scene.h"
#include <glm/gtc/matrix_transform.hpp>
namespace Iris {
	namespace Scene {
		class Camera {
		public:
			Camera(f32 width, f32 height) : m_width(width), m_height(height), m_pos(0.0f, 0.0f) {
				m_proj = glm::ortho(0.0f, m_width, m_height, 0.0f, 0.1f, 100.0f);
				m_view = glm::lookAt(glm::vec3(m_pos.x, m_pos.y, 100.0f), glm::vec3(m_pos.x, m_pos.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			void use() {
				Iris::Scene::s_activeCamera = this;
			}
			glm::mat4x4& getProjectionMatrix() {
				return m_proj;
			}
			glm::mat4x4 getViewMatrix() {
				return m_view;
			}
			glm::vec2 getPos() {
				return m_pos;
			}
			void setPos(glm::vec2 pos) {
				m_pos = pos;
				m_view = glm::lookAt(glm::vec3(m_pos.x, m_pos.y, 100.0f), glm::vec3(m_pos.x, m_pos.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			void setPosX(f32 x) {
				m_pos.x = x;
				m_view = glm::lookAt(glm::vec3(m_pos.x, m_pos.y, 100.0f), glm::vec3(m_pos.x, m_pos.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			void setPosY(f32 y) {
				m_pos.y = y;
				m_view = glm::lookAt(glm::vec3(m_pos.x, m_pos.y, 100.0f), glm::vec3(m_pos.x, m_pos.y, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			f32 getWidth() {
				return m_width;
			}
			f32 getHeight() {
				return m_height;
			}
			void setWidth(f32 width) {
				m_width = width;
				m_proj = glm::ortho(0.0f, m_width, m_height, 0.0f, 0.1f, 100.0f);
			}
			void setHeight(f32 height) {
				m_height = height;
				m_proj = glm::ortho(0.0f, m_width, m_height, 0.0f, 0.1f, 100.0f);
			}
			glm::vec2 getDimensions() {
				return glm::vec2(m_width, m_height);
			}
		private:
			f32 m_width, m_height;
			glm::vec2 m_pos;
			glm::mat4x4 m_proj;
			glm::mat4x4 m_view;
		};
		Camera createCamera(f32 width, f32 height) {
			return Camera(width, height);
		}
	}
}