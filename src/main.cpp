#define IRIS_DEBUG
#include "debugger/Debugger.h"
#include "renderer/Renderer.h"
#include "audio/Audio.h"
#include "scene/Camera.h"
#include "scene/Sprite.h"
#include "scene/SheetSprite.h"
int main() {
	{
		IRIS_TIME_FUNCTION();
		{
			Iris::Renderer::init();
			Iris::Audio::init();
			{
				IRIS_TIME_SCOPE("Window setup");
				INFO("Setting up window!");
				Iris::Renderer::createWindow(800, 800);
			}
			{
				IRIS_TIME_SCOPE("Setup");
				Iris::Renderer::Window& window = Iris::Renderer::getWindow();

				Iris::Renderer::Texture bgTex{ "assets/textures/test.jpg", Iris::Renderer::TextureFiltering::NEAREST };
				Iris::Renderer::Program defaultProgram{ "assets/shaders/defaultVertex.glsl", "assets/shaders/defaultFrag.glsl" };
				Iris::Renderer::Program testProgram{ "assets/shaders/defaultVertex.glsl", "assets/shaders/uvFrag.glsl" };
				Iris::Renderer::Program colorProgram{ "assets/shaders/defaultVertex.glsl", "assets/shaders/colorFrag.glsl" };
				
				Iris::Audio::Sound dannyMusic{ "assets/audio/Danny.wav" };
				Iris::Audio::SoundSource musicSource{ 0.05f, true };
				Iris::Audio::Sound jumpSound{ "assets/audio/jump.wav" };
				Iris::Audio::SoundSource jumpSource{ 0.05f };
				musicSource.play(dannyMusic);

				Iris::Scene::Camera camera{ 800, 800 };
				Iris::Scene::Sprite main{ bgTex , { 100, 100 } };
				Iris::Scene::Sprite bg{ bgTex , { camera.getWidth(), camera.getHeight() } };
				Iris::Scene::Sprite xSprite{ bgTex , { 1, camera.getHeight() } };
				Iris::Scene::Sprite ySprite{ bgTex , { camera.getWidth(), 1 } };
				bool uvTest = false;
				glm::vec2 pos{ 0, 0 };
				glm::vec2 vel{ 0, 0 };
				do {
					IRIS_TIME_SCOPE("Main loop");
					if (vel.x > 0.5f || vel.x < -0.5f) vel.x = fmax(fmin(vel.x, 0.5f), -0.5f);
					vel.y += 0.002f * 0.5f * window.getDeltaTime();
					pos += vel * window.getDeltaTime();
					vel.y += 0.002f * 0.5f * window.getDeltaTime();
					if (pos.y >= 700.0f) {
						vel.y = 0.0f;
						pos.y = 700.0f;
					}
					if (vel.y == 0.0f) vel.x *= powf(0.99f, window.getDeltaTime());
					f32 blend = powf(0.995f, window.getDeltaTime());
					camera.getPos() = camera.getPos() * blend + (pos - glm::vec2{ camera.getWidth() / 2.0f, camera.getHeight() / 2.0f }) * (1.0f - blend);
					window.clear();
					window.setName("{}ms", window.getDeltaTime());
					if (window.getKey(GLFW_KEY_F1) == Iris::Renderer::KeyState::PRESSED) {
						uvTest = !uvTest;
					}
					if (window.getKey(GLFW_KEY_A) == Iris::Renderer::KeyState::DOWN) {
						vel.x -= 0.03f * window.getDeltaTime();
					}
					if (window.getKey(GLFW_KEY_D) == Iris::Renderer::KeyState::DOWN) {
						vel.x += 0.03f * window.getDeltaTime();
					}
					if (window.getKey(GLFW_KEY_SPACE) == Iris::Renderer::KeyState::DOWN && vel.y == 0.0f) {
						vel.y -= 1.2f;
						jumpSource.play(jumpSound);
					}
					colorProgram.use();
					camera.use();
					colorProgram.setVec3("color", glm::vec3(0.18, 0.13, 0.26));
					bg.draw({ 0, 0 });
					colorProgram.setVec3("color", glm::vec3(0.27, 0.16, 0.5));
					for (f32 x = 0.0f; x < camera.getWidth(); x += 40.0f)
						xSprite.draw({ x, 0 });
					for (f32 y = 0.0f; y < camera.getHeight(); y += 40.0f)
						ySprite.draw({ 0, y });

					if (!uvTest)
						defaultProgram.use();
					else
						testProgram.use();
					camera.use();
					main.draw(pos);

					window.swapBuffers(); 
					window.pollEvents();
				} while (!window.shouldClose() && window.getKey(GLFW_KEY_ESCAPE) == Iris::Renderer::KeyState::UP);
			}
			{
				IRIS_TIME_SCOPE("Renderer cleanup");
				Iris::Renderer::destroy();
				Iris::Audio::destroy();
			}
		}
	}
	Iris::Debugger::save("debug.txt");
	Iris::Debugger::clean();
	return 0;
}