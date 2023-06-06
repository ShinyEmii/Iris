#pragma once
#include "../debugger/Debugger.h"
#include "../utils/units.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Renderer.h"
namespace Iris {
	namespace Renderer {
		class Program;
		static Program* s_currentProgram;
		class Program {
		public:
			Program(const char* vertex, const char* fragment) {
				u32 vertexID = glCreateShader(GL_VERTEX_SHADER);
				u32 fragID = glCreateShader(GL_FRAGMENT_SHADER);
				int Result = GL_FALSE;
				int InfoLogLength;
				auto compile = [&](u32& id, const char* input) {
					FILE* src;
					fopen_s(&src, input, "rb");
					if (src == nullptr) {
						ERROR("Shader ( \"{}\" ) doesn't exist!", input);
						return;
					}
					fseek(src, 0, SEEK_END);
					u32 size = ftell(src);
					fseek(src, 0, SEEK_SET);
					char* source = new char[(u64)size + 1];
					fread_s(source, (u64)size + 1, size, 1, src);
					source[size] = '\0';
					glShaderSource(id, 1, &source, NULL);
					glCompileShader(id);
					glGetShaderiv(id, GL_COMPILE_STATUS, &Result);
					glGetShaderiv(id, GL_INFO_LOG_LENGTH, &InfoLogLength);
					if (InfoLogLength > 0) {
						char* error = new char[(i64)InfoLogLength + 1];
						glGetShaderInfoLog(id, InfoLogLength, NULL, &error[0]);
						ERROR("Shader ( \"{}\" ) compilation failed!\n{}", input, &error[0]);
						delete[] error;
						return;
					}
				};
				compile(vertexID, vertex);
				compile(fragID, fragment);
				m_id = glCreateProgram();
				glAttachShader(m_id, vertexID);
				glAttachShader(m_id, fragID);
				glLinkProgram(m_id);
				glGetProgramiv(m_id, GL_LINK_STATUS, &Result);
				glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
				if (InfoLogLength > 0) {
					char* error = new char[(i64)InfoLogLength + 1];
					glGetProgramInfoLog(m_id, InfoLogLength, NULL, &error[0]);
					ERROR("Program ( \"{}\", \"{}\" ) compilation failed!\n{}", vertex, fragment, &error[0]);
					delete[] error;
					return;
				}
				glDetachShader(m_id, vertexID);
				glDetachShader(m_id, fragID);
				glDeleteShader(vertexID);
				glDeleteShader(fragID);
				INFO("Program ( \"{}\", \"{}\" ) loaded!", vertex, fragment);
			};
			~Program() {
				glDeleteProgram(m_id);
			}
			void use() {
				glUseProgram(m_id);
				s_currentProgram = this;
			};
			void setMatrix4x4(const char* location, glm::mat4x4 value) {
				glUniformMatrix4fv(glGetUniformLocation(m_id, location), 1, GL_FALSE, &value[0][0]);
			}
			void setInt(const char* location, int value) {
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
			uint32_t m_id;
		};
	}
}