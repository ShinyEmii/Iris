#pragma once
#include "../logger/Logger.h"
#include "ShaderData.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
namespace Iris {
	class Shader {
	public:
		Shader(): m_id(0) {}
		Shader(Renderer::ShaderData shader)
			: m_id(0) {
			uint32_t vertexID = glCreateShader(GL_VERTEX_SHADER);
			uint32_t fragID = glCreateShader(GL_FRAGMENT_SHADER);
			const char* vertexData = shader.vertexData.data();
			const char* fragData = shader.fragmentData.data();
			int InfoLogLength;
			glShaderSource(vertexID, 1, &vertexData, NULL);
			glCompileShader(vertexID);
			glGetShaderiv(vertexID, GL_INFO_LOG_LENGTH, &InfoLogLength);
			if (InfoLogLength > 0) {
				char* error = new char[InfoLogLength + 1];
				glGetShaderInfoLog(vertexID, InfoLogLength, NULL, &error[0]);
				ERROR("Vertex shader failed to compile!\n{}", &error[0]);
				delete[] error;
				return;
			}

			glShaderSource(fragID, 1, &fragData, NULL);
			glCompileShader(fragID);
			glGetShaderiv(fragID, GL_INFO_LOG_LENGTH, &InfoLogLength);
			if (InfoLogLength > 0) {
				char* error = new char[InfoLogLength + 1];
				glGetShaderInfoLog(fragID, InfoLogLength, NULL, &error[0]);
				ERROR("Vertex shader failed to compile!\n{}", &error[0]);
				delete[] error;
				return;
			}

			m_id = glCreateProgram();
			glAttachShader(m_id, vertexID);
			glAttachShader(m_id, fragID);
			glLinkProgram(m_id);
			glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
			if (InfoLogLength > 0) {
				char* error = new char[InfoLogLength + 1];
				glGetProgramInfoLog(m_id, InfoLogLength, NULL, &error[0]);
				ERROR("Shader failed to compile!\n{}", &error[0]);
				delete[] error;
				return;
			}

			glDetachShader(m_id, vertexID);
			glDetachShader(m_id, fragID);
			glDeleteShader(vertexID);
			glDeleteShader(fragID);

			INFO("Program loaded!");
		};
		~Shader() {
			glDeleteProgram(m_id);
		}
		void use() {
			glUseProgram(m_id);
		};
		void setMatrix4x4(const char* location, glm::mat4x4 value) {
			glUniformMatrix4fv(glGetUniformLocation(m_id, location), 1, GL_FALSE, &value[0][0]);
		}
		void setInt(const char* location, int32_t value) {
			glUniform1i(glGetUniformLocation(m_id, location), value);
		}
		void setFloat(const char* location, float value) {
			glUniform1f(glGetUniformLocation(m_id, location), value);
		}
		void setVec3(const char* location, glm::vec3 value) {
			glUniform3fv(glGetUniformLocation(m_id, location), 1, &value[0]);
		}
		void setVec2(const char* location, glm::vec2 value) {
			glUniform2fv(glGetUniformLocation(m_id, location), 1, &value[0]);
		}
	private:
		GLuint m_id;
	};
}