#pragma once
#include "Window.h"
#include "Texture.h"
#include "Program.h"
#include "FrameBuffer.h"
#include "Mesh.h"
#include "../debugger/Debugger.h"
namespace Iris {
	namespace Renderer {
		static Window* s_window;
		static Mesh* s_quad;
		i8 init() {
			if (!glfwInit()) {
				ERROR("Failed to initialize GLFW!");
				return 0;
			}
			glfwWindowHint(GLFW_SAMPLES, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

			glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			GLFWwindow* info = glfwCreateWindow(1, 1, "IRIS", NULL, NULL);
			glfwMakeContextCurrent(info);
			if (glewInit() != GLEW_OK) {
				ERROR("Failed to initialize GLEW!");
				return 0;
			}
			s_monitors = glfwGetMonitors((int*)&s_monitorCount);
			s_videoMode = glfwGetVideoMode(s_monitors[0]);

			glfwDestroyWindow(info);
			glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE);
			glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
			s_glInit = true;
			return 1;
		}
		Window& createWindow(u16 width, u16 height, const char* name = "Iris Application", WindowMode mode = WindowMode::WINDOWED, bool vsync = false) {
			if (s_window != nullptr) {
				ERROR("Failed to create new window. Window already exists!");
				return *s_window;
			}
			s_window = new Window(width, height, name, mode, vsync);
			Iris::Renderer::MeshData quadData = {
				{ 0, 1, 2, 0, 2, 3 },
				{
					{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
					{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
					{ { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
					{ { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } }
				}
			};
			s_quad = new Mesh(quadData);
			return *s_window;
		} 
		Window& getWindow() {
			if (s_window == nullptr) {
				ERROR("Window hasn't been created!");
			}
			return *s_window;
		}

		void destroy() {
			if (s_window != nullptr) {
				delete s_window;
				delete s_quad;
				s_window = nullptr;
			}
		}
	}
}