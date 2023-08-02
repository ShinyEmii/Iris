#pragma once
#include "../debugger/Debugger.h"
#include "../utils/utility.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../assets/Package.h"
#include "Renderer.h"
namespace Iris {
	namespace Renderer {
		class Program;
		static Program* s_activeProgram{0};
		class Program {
		public:
			Program()
				: m_id(0) {

			}
			Program(char* vertexData, char* fragmentData, bool free = true) 
				: m_id(0) {
				u32 vertexID = glCreateShader(GL_VERTEX_SHADER);
				u32 fragID = glCreateShader(GL_FRAGMENT_SHADER);
				int InfoLogLength;
				glShaderSource(vertexID, 1, &vertexData, NULL);
				glCompileShader(vertexID);
				glGetShaderiv(vertexID, GL_INFO_LOG_LENGTH, &InfoLogLength);
				if (InfoLogLength > 0) {
					char* error = new char[InfoLogLength + 1];
					glGetShaderInfoLog(vertexID, InfoLogLength, NULL, &error[0]);
					ERROR("Vertex shader compilation failed!\n{}", &error[0]);
					delete[] error;
					return;
				}
				glShaderSource(fragID, 1, &fragmentData, NULL);
				glCompileShader(fragID);
				glGetShaderiv(fragID, GL_INFO_LOG_LENGTH, &InfoLogLength);
				if (InfoLogLength > 0) {
					char* error = new char[InfoLogLength + 1];
					glGetShaderInfoLog(fragID, InfoLogLength, NULL, &error[0]);
					ERROR("Vertex shader compilation failed!\n{}", &error[0]);
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
					ERROR("Program compilation failed!\n{}", &error[0]);
					delete[] error;
					return;
				}
				glDetachShader(m_id, vertexID);
				glDetachShader(m_id, fragID);
				glDeleteShader(vertexID);
				glDeleteShader(fragID);
				if (free) {
					delete[] vertexData;
					delete[] fragmentData;
				}
				INFO("Program loaded!");
			};
			~Program() {
				glDeleteProgram(m_id);
			}
			void use() {
				glUseProgram(m_id);
				s_activeProgram = this;
			};
			void setMatrix4x4(const char* location, glm::mat4x4 value) {
				glUniformMatrix4fv(glGetUniformLocation(m_id, location), 1, GL_FALSE, &value[0][0]);
			}
			void setInt(const char* location, i32 value) {
				glUniform1i(glGetUniformLocation(m_id, location), value);
			}
			void setFloat(const char* location, f32 value) {
				glUniform1f(glGetUniformLocation(m_id, location), value);
			}
			void setVec3(const char* location, glm::vec3 value) {
				glUniform3fv(glGetUniformLocation(m_id, location), 1, &value[0]);
			}
			void setVec2(const char* location, glm::vec2 value) {
				glUniform2fv(glGetUniformLocation(m_id, location), 1, &value[0]);
			}
		private:
			u32 m_id;
		};
		Program loadProgram(const char* vertex, const char* fragment) {
			INFO("Loading Program from files \"{}\" and \"{}\".", vertex, fragment);
			FILE* f;
			fopen_s(&f, vertex, "rb");
			if (f == nullptr) {
				ERROR("Failed to load program! File \"{}\" doesn't exist!", vertex);
				return Program();
			}
			size_t size;
			fseek(f, 0, SEEK_END);
			size = ftell(f);
			fseek(f, 0, SEEK_SET);
			char* vertexData = new char[size + 1] {0};
			fread(vertexData, size, 1, f);
			fclose(f);

			fopen_s(&f, fragment, "rb");
			if (f == nullptr) {
				delete[] vertexData;
				ERROR("Failed to load program! File \"{}\" doesn't exist!", fragment);
				return Program();
			}
			fseek(f, 0, SEEK_END);
			size = ftell(f);
			fseek(f, 0, SEEK_SET);
			char* fragmentData = new char[size + 1] {0};
			fread(fragmentData, size, 1, f);
			fclose(f);
			return Program(vertexData, fragmentData);
		}
		Program loadProgram(Assets::Package& package, const char* vertex, const char* fragment) {
			INFO("Loading Program from package \"{}\" with names \"{}\" and \"{}\".", package.getName(), vertex, fragment);
			Assets::Asset v = package.getAsset(vertex);
			Assets::Asset f = package.getAsset(fragment);
			if (v.size == 0 || f.size == 0) return Program();
			return Program(v.data, f.data, false);
		}
	}
}