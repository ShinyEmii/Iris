#define IRIS_DEBUG
#include "debugger/Debugger.h"
#include "renderer/Renderer.h"
#include "audio/Audio.h"
#include "scene/Camera.h"
#include "scene/Sprite.h"
#include "scene/SheetSprite.h"
#include "serializer/Serliazier.h"
class Player {
public:
	f32 x = 0, y = 0, vx = 0, vy = 0;
};
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
				
				Iris::Audio::Sound dannyMusic{ "assets/audio/C418 - Kyoto.wav" };
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

				Iris::Serializer::registerType<Player>(Iris::Serializer::MetaType::F32, Iris::Serializer::MetaType::F32, Iris::Serializer::MetaType::F32, Iris::Serializer::MetaType::F32);
				Player player;
				Iris::Serializer::loadMetaData("save.dat");
				Iris::Serializer::loadFromMetaData(&player, "player");
				do {
					IRIS_TIME_SCOPE("Main loop");
					if (player.vx > 0.5f || player.vx < -0.5f) player.vx = fmax(fmin(player.vx, 0.5f), -0.5f);
					player.vy += 0.002f * 0.5f * window.getDeltaTime();
					player.x += player.vx * window.getDeltaTime();
					player.y += player.vy * window.getDeltaTime();
					player.vy += 0.002f * 0.5f * window.getDeltaTime();
					if (player.y >= 700.0f) {
						player.vy = 0.0f;
						player.y = 700.0f;
					}
					if (player.x >= 700.0f) {
						player.vx = 0.0f;
						player.x = 700.0f;
					}
					if (player.x <= 0.0f) {
						player.vx = 0.0f;
						player.x = 0.0f;
					}
					if (player.vy == 0.0f) player.vx *= powf(0.99f, window.getDeltaTime());
					f32 blend = powf(0.995f, window.getDeltaTime());
					camera.getPos() = camera.getPos() * blend + (glm::vec2{ player.x, player.y } - glm::vec2{ camera.getWidth() / 2.0f, camera.getHeight() / 2.0f }) * (1.0f - blend);
					window.clear();
					window.setName("{}ms", window.getDeltaTime());
					if (window.getKey(GLFW_KEY_F1) == Iris::Renderer::KeyState::PRESSED) {
						uvTest = !uvTest;
					}
					if (window.getKey(GLFW_KEY_A) == Iris::Renderer::KeyState::DOWN) {
						player.vx -= 0.03f * window.getDeltaTime();
					}
					if (window.getKey(GLFW_KEY_D) == Iris::Renderer::KeyState::DOWN) {
						player.vx += 0.03f * window.getDeltaTime();
					}
					if (window.getKey(GLFW_KEY_SPACE) == Iris::Renderer::KeyState::DOWN && player.vy == 0.0f) {
						player.vy -= 1.2f;
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
					main.draw({player.x, player.y});

					window.swapBuffers(); 
					window.pollEvents();
				} while (!window.shouldClose() && window.getKey(GLFW_KEY_ESCAPE) == Iris::Renderer::KeyState::UP);
				Iris::Serializer::createMetaData(player, "player");
				Iris::Serializer::saveMetaData("save.dat");
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