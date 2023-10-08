#define IRIS_DEBUG
#define IRIS_IGNORE_WARNINGS
#include "logger/Logger.h"
#include "assets/Loader.h"
#include "assets/Package.h"
#include "audio/AudioDevice.h"
#include "renderer/VideoDevice.h"
#include "physics/Physics.h"
int main() {
	Iris::Audio::createDevice();
	Iris::Renderer::createDevice(1280, 720);
	Iris::Loader::start();
	Iris::Physics::start();
	//Iris::Loader::loadPackageAsync("package.ipk");
	Iris::Loader::loadSoundAsync("assets/audio/C418 - Kyoto.wav");
	Iris::Loader::loadSoundAsync("assets/audio/jump.wav");
	Iris::Loader::loadTextureAsync("assets/textures/test.jpg");
	//Iris::Loader::createPackage("package.ipk");

	Iris::SoundSource soundSource;
	Iris::Renderer::MeshData quadData = {
	{ 0, 1, 2, 0, 2, 3 },
	{
		{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
		{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
		{ { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } }
	}
	};
	Iris::Mesh quad(quadData);
	Iris::Renderer::ShaderData shaderData = {
		true,
		"#version 330 core\n\
		layout(location = 0) in vec3 vertex;\n\
		layout(location = 1) in vec2 uvs;\n\
		out vec2 UV;\n\
		void main() {\n\
			gl_Position = vec4(vertex, 1.0f);\n\
			UV = clamp(uvs, vec2(0.0f), vec2(1.0f));\n\
		}",
		"#version 330 core\n\
		in vec2 UV;\n\
		uniform sampler2D albedo;\n\
		out vec4 outColor;\n\
		void main() {\n\
			vec2 uvs = UV;\n\
			outColor = texture(albedo, uvs);\n\
		}"
	};
	Iris::Shader shader(shaderData);

	do {
		Iris::Loader::update();
		Iris::Renderer::pollEvents();

		if (Iris::getKey(GLFW_KEY_F1) == Iris::KeyState::DOWN) {
			LOG("{}", Iris::getTexture("assets/textures/test.jpg").get().getTexture());
			LOG("LOADING {:.2f}% {} Queued {} Done {} Failed", Iris::Loader::getLoadedRatio() * 100.0f, Iris::Loader::getQueued(), Iris::Loader::getDone(), Iris::Loader::getFailed());
		}
		if (Iris::getKey(GLFW_KEY_SPACE) == Iris::KeyState::DOWN) {
			soundSource.forcePlay("assets/audio/jump.wav");
		}
		if (Iris::getKey(GLFW_KEY_ESCAPE) == Iris::KeyState::DOWN) {
			Iris::Renderer::stop();
		}
		Iris::Renderer::clear();
		shader.use();
		Iris::getTexture("assets/textures/test.jpg").get().bindTexture(0);
		shader.setInt("albedo", 0);
		quad.draw();
		Iris::Renderer::swapBuffers();
	} while (!Iris::Renderer::shouldClose());

	Iris::Physics::stop();
	Iris::Loader::stop();
	Iris::Renderer::destroyDevice();
	Iris::Audio::destroyDevice();
	Iris::Logger::save("debug.txt");
	return 5;

}