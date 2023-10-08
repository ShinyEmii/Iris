#pragma once
#include "../logger/Logger.h"
#include "../assets/Resources.h"
#include "texture/Texture.h"
#include "mesh/Mesh.h"
#include "shader/Shader.h"
namespace Iris {
	enum class WindowMode {
		WINDOWED,
		BORDERLESS,
		FULLSCREEN
	};
	enum class KeyState {
		RELEASED,
		UP,
		PRESSED,
		DOWN
	};
	namespace Renderer {
		static bool s_isRunning = false;
		static bool s_deviceCreated = false;
		static GLFWmonitor** s_monitors = nullptr;
		static uint64_t s_monitorCount = 0;
		const static GLFWvidmode* s_videoMode = nullptr;
		static GLFWwindow* s_window = nullptr;
		static uint16_t s_width, s_height;
		static WindowMode s_windowMode;
		static bool s_verticalSync = false;
		static KeyState s_keyBuffer[GLFW_KEY_LAST];
		static KeyState s_mouseBuffer[GLFW_MOUSE_BUTTON_LAST];
		static glm::vec2 s_mousePos = { 0, 0 };
		static float s_deltaTime = 0.0f;
		static double s_time = 0.0;
		static std::chrono::system_clock::time_point s_lastFrame;
	}
	glm::vec2 getMousePos() {
		return Renderer::s_mousePos;
	}
	KeyState getKey(uint16_t key) {
		if (key >= GLFW_KEY_LAST) return KeyState::RELEASED;
		return Renderer::s_keyBuffer[key];
	}
	KeyState getMouseButton(uint8_t button) {
		if (button >= GLFW_MOUSE_BUTTON_LAST) return KeyState::RELEASED;
		return Renderer::s_mouseBuffer[button];
	}
	namespace Renderer {
		GLFWmonitor** getMonitors() {
			if (!s_deviceCreated) {
				ERROR("Video device hasn't been initialized!");
				return nullptr;
			}
			return s_monitors;
		}
		const GLFWvidmode* getVideoMode() {
			if (!s_deviceCreated) {
				ERROR("Video device hasn't been initialized!");
				return nullptr;
			}
			return s_videoMode;
		}
		uint64_t getMonitorCount() {
			if (!s_deviceCreated) {
				ERROR("Video device hasn't been initialized!");
				return 0;
			}
			return s_monitorCount;
		}
		void refreshWindow() {
			glfwSwapInterval(s_verticalSync ? 1 : 0);
			switch (s_windowMode) {
			case WindowMode::WINDOWED:
				glfwSetWindowMonitor(s_window, NULL, getVideoMode()->width / 2 - s_width / 2, getVideoMode()->height / 2 - s_height / 2, s_width, s_height, GLFW_DONT_CARE);
				break;
			case WindowMode::BORDERLESS:
				glfwSetWindowMonitor(s_window, glfwGetPrimaryMonitor(), 0, 0, getVideoMode()->width, getVideoMode()->height, GLFW_DONT_CARE);
				break;
			case WindowMode::FULLSCREEN:
				glfwSetWindowMonitor(s_window, glfwGetPrimaryMonitor(), 0, 0, s_width, s_height, GLFW_DONT_CARE);
				break;
			}
		}
		template <typename... Args>
		void setName(const char* format, Args&&... args) {
			glfwSetWindowTitle(s_window, std::vformat(format, std::make_format_args(args...)).c_str());
		}
		void clear() {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		void pollEvents() {
			s_deltaTime = (std::chrono::duration<float, std::milli>(std::chrono::system_clock::now() - s_lastFrame)).count();
			s_lastFrame = std::chrono::system_clock::now();
			s_time += s_deltaTime;
			for (uint16_t i = 0; i < GLFW_KEY_LAST; i++) {
				if (s_keyBuffer[i] == KeyState::DOWN) s_keyBuffer[i] = KeyState::PRESSED;
				if (s_keyBuffer[i] == KeyState::UP) s_keyBuffer[i] = KeyState::RELEASED;
			}
			for (uint8_t i = 0; i < 8; i++) {
				if (s_mouseBuffer[i] == KeyState::DOWN) s_mouseBuffer[i] = KeyState::PRESSED;
				if (s_mouseBuffer[i] == KeyState::UP) s_mouseBuffer[i] = KeyState::RELEASED;
			}
			glfwPollEvents();
		}
		void swapBuffers() {
			glfwSwapBuffers(s_window);
		}
		float getDeltaTime() {
			if (s_deltaTime == 0.0f) return 0.01f;
			return s_deltaTime;
		}
		double getTime() {
			return s_time;
		}
		void stop() {
			if (!s_isRunning) {
				ERROR("Tried to stop video device without starting it!");
				return;
			}
			s_isRunning = false;
		}
		bool shouldClose() {
			return glfwWindowShouldClose(s_window) || !s_isRunning;
		}
		void setResolution(uint16_t width, uint16_t height) {
			s_width = width;
			s_height = height;
			refreshWindow();
		}
		uint16_t getWidth() {
			return s_width;
		}
		uint16_t getHeight() {
			return s_height;
		}
		glm::vec2 getDimensions() {
			return glm::vec2(s_width, s_height);
		}
		void setWindowMode(WindowMode windowMode) {
			s_windowMode = windowMode;
			refreshWindow();
		}
		void setVerticalSync(bool vsync) {
			s_verticalSync = vsync;
			refreshWindow();
		}
		void lockMouse(bool locked) {
			glfwSetInputMode(s_window, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
		}
		void createDevice(uint16_t width, uint16_t height, std::string_view name = "Iris Application", WindowMode mode = WindowMode::WINDOWED, bool verticalSync = false) {
			if (s_deviceCreated) {
				ERROR("Video Device was already initialized!");
				return;
			}
			if (!glfwInit()) {
				ERROR("Failed to initialize GLFW!");
				return;
			}
			glfwWindowHint(GLFW_SAMPLES, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
			s_width = width;
			s_height = height;
			s_windowMode = mode;
			s_verticalSync = verticalSync;
			s_monitors = glfwGetMonitors((int*)&s_monitorCount);
			s_videoMode = glfwGetVideoMode(s_monitors[0]);
			s_window = glfwCreateWindow(s_width, s_height, name.data(), NULL, NULL);
			if (s_window == NULL) {
				ERROR("Failed to open GLFW window!");
				return;
			}
			glfwMakeContextCurrent(s_window);
			if (glewInit() != GLEW_OK) {
				ERROR("Failed to initialize GLEW!");
				return;
			}
			INFO("Video device initialized.");
			INFO("Using Video Device: {}", (const char*)glGetString(GL_RENDERER));
			glfwMakeContextCurrent(s_window);
			if (glfwRawMouseMotionSupported()) {
				glfwSetInputMode(s_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
			}
			glfwSetInputMode(s_window, GLFW_STICKY_KEYS, GL_TRUE);
			glfwSetWindowSizeCallback(s_window, [](GLFWwindow* window, int width, int height) {
				refreshWindow();
			});
			glfwSetKeyCallback(s_window, [](GLFWwindow* window, int key, int scancode, int action, int mode) {
				if (key >= GLFW_KEY_LAST) return;
				if (action == 1)
					s_keyBuffer[key] = KeyState::DOWN;
				else if (action == 0)
					s_keyBuffer[key] = KeyState::UP;
			});
			glfwSetCursorPosCallback(s_window, [](GLFWwindow* window, double x, double y) {
				s_mousePos = { x, y };
			});
			glfwSetMouseButtonCallback(s_window, [](GLFWwindow* window, int button, int action, int mods) {
				if (button >= GLFW_MOUSE_BUTTON_LAST) return;
				if (action == 1)
					s_mouseBuffer[button] = KeyState::DOWN;
				else if (action == 0)
					s_mouseBuffer[button] = KeyState::UP;
			});
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			glClearColor(0.0, 0.0, 0.0, 0.0);

			s_deviceCreated = true;
			s_isRunning = true;
			Resource::s_defaultTexture.setAsset(new Texture());
		}
		void destroyDevice() {
			if (!s_deviceCreated) {
				ERROR("Tried to destroy Video Device without initializing it!");
				return;
			}
			if (s_window != nullptr) {
				glfwDestroyWindow(s_window);
			}
			s_deviceCreated = false;
			INFO("Video Device destroyed");
		}
	}
}