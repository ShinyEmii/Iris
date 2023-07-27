#pragma once
#include "../debugger/Debugger.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
namespace Iris {
	namespace Renderer {
		class Mesh;
		static Mesh* s_quadMesh;
		struct MeshPoint {
			glm::vec3 pos; 
			glm::vec2 uv;
		};
		struct MeshData {
			std::vector<u32> indices;
			std::vector<MeshPoint> points;
		};
		class Mesh {
		public:
			Mesh(MeshData& data) 
				: m_size(0), m_vbo(0), m_vao(0), m_ebo(0) {
				if (data.points.empty() || data.indices.empty()) {
					ERROR("Failed to create Mesh because given data was empty!");
					return;
				}
				m_size = (GLsizei)data.indices.size();
				glGenVertexArrays(1, &m_vao);
				glGenBuffers(1, &m_vbo);
				glGenBuffers(1, &m_ebo);
				glBindVertexArray(m_vao);
				glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
				glBufferData(GL_ARRAY_BUFFER, data.points.size() * sizeof(MeshPoint), &data.points[0], GL_STATIC_DRAW);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(u32), &data.indices[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshPoint), (void*)0);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(MeshPoint), (void*)offsetof(MeshPoint, uv));
				INFO("Mesh successfully created.");
			}
			~Mesh() {
				glDeleteVertexArrays(1, &m_vao);
				glDeleteBuffers(1, &m_vbo);
				glDeleteBuffers(1, &m_ebo);
				m_size = 0;
			}
			void draw() {
				if (m_size == 0) {
					ERROR("Failed to draw mesh! Mesh is invalid!");
					return;
				}
				glBindVertexArray(m_vao);
				glDrawElements(GL_TRIANGLES, m_size, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
				return;
			}
		private:
			GLsizei m_size;
			GLuint m_vbo, m_vao, m_ebo;
		};
		//Mesh& createMeshFromPoints(std::vector<MeshPoint> points) {
		//}
	}
}