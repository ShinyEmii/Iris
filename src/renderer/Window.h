#pragma once
#include "../utils/units.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
namespace Iris {
	namespace Renderer {
		static bool s_glInit = false;
		static GLFWmonitor** s_monitors;
		static u64 s_monitorCount;
		const static GLFWvidmode* s_videoMode;
		GLFWmonitor** getMonitors() {
			if (!s_glInit) {
				ERROR("Renderer hasn't been initialized!");
			}
			return s_monitors;
		}
		const GLFWvidmode* getVideoMode() {
			if (!s_glInit) {
				ERROR("Renderer hasn't been initialized!");
			}
			return s_videoMode;
		}
		u64 getMonitorCount() {
			if (!s_glInit) {
				ERROR("Renderer hasn't been initialized!");
				return 0;
			}
			return s_monitorCount;
		}
		enum class WindowMode {
			WINDOWED_MODE,
			BORDERLESS_MODE,
			FULLSCREEN_MODE
		};
		class Window {
		public:
			Window(u16 width, u16 height, const char* name, WindowMode mode, bool vsync)
				: m_running(true), m_width(width), m_height(height), m_windowMode(mode), m_verticalSync(vsync) {
				memset(m_keyBuffer, 0, 512);
				memset(m_mouseBuffer, 0, 8);
				m_window = glfwCreateWindow(m_width, m_height, name, NULL, NULL);
				if (m_window == NULL) {
					ERROR("Failed to open GLFW window!");
					return;
				}
				glfwMakeContextCurrent(m_window);
				if (glfwRawMouseMotionSupported()) {
					glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
				}
				glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);
				glfwSetWindowUserPointer(m_window, this);
				glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
					auto& self = *(Window*)(glfwGetWindowUserPointer(window));
					self.setResolution(width, height);
					self.refreshWindow();
				});
				glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mode) {
					auto& self = *(Window*)(glfwGetWindowUserPointer(window));
					if (key >= 512) return;
					if (action == 1) {
						self.m_keyBuffer[key] = true;
					}
					else if (action == 0) {
						self.m_keyBuffer[key] = false;
					}
				});
				glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double x, double y) {
					auto& self = *(Window*)(glfwGetWindowUserPointer(window));
					self.m_mousePos = { x, y };
				});
				glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
					auto& self = *(Window*)(glfwGetWindowUserPointer(window));
					if (action == 1) {
						self.m_mouseBuffer[button] = true;
					}
					else if (action == 0) {
						self.m_mouseBuffer[button] = false;
					}
				});
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glEnable(GL_BLEND);
				refreshWindow();
				glClearColor(0.0, 0.0, 0.0, 0.0);
			}
			~Window() {
				if (m_window == nullptr) {
					ERROR("Failed to destroy window! The pointer is null");
					return;
				}
				glfwDestroyWindow(m_window);
			}
			void stop() {
				m_running = false;
			}
			void setName(const char* name) {
				glfwSetWindowTitle(m_window, name);
			}
			void clear() {
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}
			void pollEvents() {
				glfwPollEvents();
			}
			void swapBuffers() {
				glfwSwapBuffers(m_window);
			}
			bool shouldClose() {
				return !m_running || glfwWindowShouldClose(m_window);
			}
			void setResolution(u16 width, u16 height) {
				m_width = width;
				m_height = height;
				refreshWindow();
			}
			u16 getWidth() {
				return m_width;
			}
			u16 getHeight() {
				return m_height;
			}
			void setWindowMode(WindowMode windowMode) {
				m_windowMode = windowMode;
				refreshWindow();
			}
			void setVerticalSync(bool vsync) {
				m_verticalSync = vsync;
				refreshWindow();
			}
			void lockMouse(bool locked) {
				glfwSetInputMode(m_window, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
			}
			glm::vec2 getMousePos() {
				return m_mousePos;
			}
			bool getKey(u16 key) {
				return m_keyBuffer[key];
			}
			bool getMouseButton(u8 button) {
				return m_mouseBuffer[button];
			}
		private:
			void refreshWindow() {
				glfwSwapInterval(m_verticalSync ? 1 : 0);
				switch (m_windowMode) {
				case WindowMode::WINDOWED_MODE:
					glfwSetWindowMonitor(m_window, NULL, getVideoMode()->width / 2 - m_width / 2, getVideoMode()->height / 2 - m_height / 2, m_width, m_height, GLFW_DONT_CARE);
					break;
				case WindowMode::BORDERLESS_MODE:
					glfwSetWindowMonitor(m_window, glfwGetPrimaryMonitor(), 0, 0, getVideoMode()->width, getVideoMode()->height, GLFW_DONT_CARE);
					break;
				case WindowMode::FULLSCREEN_MODE:
					glfwSetWindowMonitor(m_window, glfwGetPrimaryMonitor(), 0, 0, m_width, m_height, GLFW_DONT_CARE);
					break;
				}
			}
			bool m_verticalSync;
			WindowMode m_windowMode;
			bool m_running;
			u16 m_width, m_height;

			glm::vec2 m_mousePos;
			bool m_keyBuffer[512];
			bool m_mouseBuffer[8];
			GLFWwindow* m_window;
		};
	}
}