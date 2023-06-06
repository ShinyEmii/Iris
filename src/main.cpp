#define IRIS_DEBUG
#include "debugger/Debugger.h"
#include "renderer/Renderer.h"
#include "scene/Camera.h"
#include "scene/Sprite.h"
#include "scene/SheetSprite.h"
int main() {
	{
		IRIS_TIME_FUNCTION();
		{
			Iris::Renderer::init();
			{
				IRIS_TIME_SCOPE("Window setup");
				INFO("Setting up window!");
				Iris::Renderer::createWindow(928, 793, "EXAMPLE APP", Iris::Renderer::WindowMode::WINDOWED_MODE);
			}
			{
				IRIS_TIME_SCOPE("Main loop");
				Iris::Renderer::Window& window = Iris::Renderer::getWindow();

				Iris::Renderer::Texture bgTex{ "assets/textures/bg.png", Iris::Renderer::TextureFiltering::Nearest };
				Iris::Renderer::Texture sheet{ "assets/textures/LightBandit.png", Iris::Renderer::TextureFiltering::Nearest, Iris::Renderer::TextureWrapping::Mirrored };

				Iris::Renderer::Program defaultProgram{ "assets/shaders/vertex.glsl", "assets/shaders/frag.glsl" };
				Iris::Renderer::Program sheetProgram{ "assets/shaders/sheetVertex.glsl", "assets/shaders/sheetFrag.glsl" };

				Iris::Scene::Camera camera = Iris::Scene::createCamera(928, 793);
				Iris::Scene::Sprite bg{ bgTex , {1856, 1586} };
				Iris::Scene::SheetSprite player{ sheet, {48, 48}, {96, 96} };

				float frame = 0.0f;
				float x = 0;
				bool flip = false;
				bool walking = false;
				do {
					frame += 0.002f;
					if (window.getKey(GLFW_KEY_A)) {
						x -= 0.07f;
						flip = false;
						if (!walking) frame = 0.0f;
						walking = true;
					}
					if (window.getKey(GLFW_KEY_D)) {
						x += 0.07f;
						flip = true;
						if (!walking) frame = 0.0f;
						walking = true;
					}
					if ((!window.getKey(GLFW_KEY_D) && !window.getKey(GLFW_KEY_A)) || (window.getKey(GLFW_KEY_D) && window.getKey(GLFW_KEY_A))) {
						if (walking) frame = 0.0f;
						walking = false;
					}
					camera.updatePos(glm::vec2(x + 48, 0));

					window.clear();

					defaultProgram.use();
					camera.use();
					bg.draw({ -928, -430 });

					sheetProgram.use();
					camera.use();
					if (walking) player.draw({ (int)floor(frame) % 8, 3 }, { x, -310 }, flip);
					else player.draw({ (int)floor(frame) % 4, 4 }, { x, -310 }, flip);

					window.swapBuffers(); 
					window.pollEvents();
				} while (!window.shouldClose() && !window.getKey(256));
			}
			{
				IRIS_TIME_SCOPE("Renderer cleanup");
				Iris::Renderer::destroy();
			}
		}
	}
	Iris::Debugger::save("debug.txt");
	Iris::Debugger::clean();
	return 0;
}