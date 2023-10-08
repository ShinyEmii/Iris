#include "GameObject.h"
namespace Iris {
	class Camera;
	static Camera* s_activeCamera = nullptr;
	Camera& getActiveCamera() {
		if (s_activeCamera == nullptr) {
			ERROR("There isn't any active camera!");
		}
		return *s_activeCamera;
	}
	class Camera : public GameObject {
	public:
		void onCreate() {
			LOG("Camera created!");
			if (s_activeCamera == nullptr) s_activeCamera = this;
		}
		void onUpdate() {
			//LOG("Camera update!");
		}
		void onDestroy() {
			LOG("Camera destroyed!");
			if (s_activeCamera == this) s_activeCamera = nullptr;
		}
	};
}