#define IRIS_DEBUG
#include "debugger/Debugger.h"
#include "renderer/Renderer.h"
#include "audio/Audio.h"
#include "scene/Camera.h"
#include "scene/Sprite.h"
#include "scene/SheetSprite.h"
#include "serializer/Serliazier.h"
#include "assets/Package.h"
class Player {
public:
	void update(f32 delta) {
		if (vel.x > 0.5f || vel.x < -0.5f) vel.x = fmax(fmin(vel.x, 0.5f), -0.5f);
		vel += acc * 0.5f * delta;
		pos += vel * delta;
		vel += acc * 0.5f * delta;
		if (pos.y >= 720.0f) {
			vel.y = 0.0f;
			pos.y = 720.0f;
			doubleJump = true;
		}
		if (pos.x >= 720.0f) {
			vel.x = 0.0f;
			pos.x = 720.0f;
		}
		if (pos.x <= 0.0f) {
			vel.x = 0.0f;
			pos.x = 0.0f;
		}
		if (vel.y == 0.0f) vel.x *= powf(0.99f, delta);
	}
	bool doubleJump = true;
	glm::vec2 pos{ 0, 0 };
	glm::vec2 vel{ 0, 0 };
	glm::vec2 acc{ 0, 0.002f };
};
using namespace Iris;
int main() {
	Renderer::init();
	Audio::init();

	//Assets::createPackage("assets.ipk", { "assets/textures/test.jpg", "assets/shaders/defaultVertex.glsl", "assets/shaders/defaultFrag.glsl", "assets/audio/C418 - Kyoto.wav", "assets/audio/jump.wav" });
	Renderer::Window& window = Renderer::createWindow(1280, 720);
	Assets::Package pak = Assets::loadPackage("assets.ipk");

	Renderer::Texture tex = Renderer::loadTexture( pak, "assets/textures/test.jpg", Renderer::TextureFiltering::NEAREST );
	Renderer::Program defaultProgram = Renderer::loadProgram( pak, "assets/shaders/defaultVertex.glsl", "assets/shaders/defaultFrag.glsl" );

	Audio::Sound dannyMusic = Audio::loadSound( pak, "assets/audio/C418 - Kyoto.wav" );
	Audio::SoundSource musicSource{ 0.05f, true };
	Audio::Sound jumpSound = Audio::loadSound( pak, "assets/audio/jump.wav" );
	Audio::SoundSource jumpSource{ 0.05f };
	musicSource.play(dannyMusic);

	Scene::Camera camera{ (f32)window.getWidth(), (f32)window.getHeight() };
	Scene::Sprite main{ tex , { 80, 80 } };
	Scene::Sprite bg{ tex , { 800, 800 } };

	Serializer::registerType<Player>()
		.property("pos", &Player::pos)
		.property("vel", &Player::vel)
		.property("doubleJump", &Player::doubleJump);

	Player player;
	Serializer::loadMetaData("save.dat");
	Serializer::loadFromMetaData(player, "player");
	do {
		IRIS_TIME_SCOPE("Frame Time");
		player.update(window.getDeltaTime());
		camera.getPos() = deltaLerp(camera.getPos(), player.pos - (camera.getDimensions() * 0.5f), 0.995f, window.getDeltaTime());
		if (window.getKey(GLFW_KEY_A) == Renderer::KeyState::DOWN) {
			player.vel.x -= 0.01f * window.getDeltaTime();
		}
		if (window.getKey(GLFW_KEY_D) == Renderer::KeyState::DOWN) {
			player.vel.x += 0.01f * window.getDeltaTime();
		}
		if (window.getKey(GLFW_KEY_SPACE) == Renderer::KeyState::PRESSED && (player.vel.y == 0.0f || player.doubleJump)) {
			if (player.doubleJump && player.vel.y != 0.0f)
				player.doubleJump = false;
			player.vel.y = -1.2f;
			jumpSource.forcePlay(jumpSound);
		}

		window.clear();
		defaultProgram.use();
		camera.use();
		bg.draw({ 0, 0 });
		main.draw(player.pos);
		window.swapBuffers();
		window.pollEvents();
	} while (!window.shouldClose() && window.getKey(GLFW_KEY_ESCAPE) == Renderer::KeyState::UP);

	Serializer::createMetaData(player, "player");
	Serializer::saveMetaData("save.dat");

	Renderer::destroy();
	Audio::destroy();

	Debugger::save("debug.txt");
	return 0;
}