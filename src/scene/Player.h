#include "../renderer/VideoDevice.h"
#include "Scene.h"
using namespace Iris;
class Player : public GameObject {
public:
	void onCreate() {};
	void onDestroy() {};
	void onUpdate() {
		float deltaTime = Iris::Renderer::getDeltaTime();
		vel *= powf(0.99f, deltaTime);
		if (Iris::getKey(GLFW_KEY_W) == KeyState::DOWN)
			vel.y += 0.01f * deltaTime;
		if (Iris::getKey(GLFW_KEY_S) == KeyState::DOWN)
			vel.y -= 0.01f * deltaTime;
		if (Iris::getKey(GLFW_KEY_A) == KeyState::DOWN)
			vel.x -= 0.01f * deltaTime;
		if (Iris::getKey(GLFW_KEY_D) == KeyState::DOWN)
			vel.x += 0.01f * deltaTime;
		pos += vel * deltaTime;
	glm::vec2 pos;
	glm::vec2 vel;
};