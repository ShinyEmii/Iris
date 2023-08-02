#pragma once
#include "../renderer/Window.h"
namespace Iris {
	namespace Scene {
		class Camera;
		Camera* s_activeCamera = nullptr;
		Camera& getActiveCamera() {
			if (s_activeCamera == nullptr) {
				ERROR("There isn't any active camera!");
			}
			return *s_activeCamera;
		}
	}
}