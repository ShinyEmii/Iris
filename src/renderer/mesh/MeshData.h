#pragma once
#include <glm\glm.hpp>
#include "../logger/Logger.h"
#include <vector>
namespace Iris {
	namespace Renderer {
		struct MeshPoint {
			glm::vec3 pos;
			glm::vec2 uv;
		};
		struct MeshData {
			std::vector<uint32_t> indices;
			std::vector<MeshPoint> points;
		};
	}
	//Renderer::MeshData createMeshDataFromPoints(std::vector<Renderer::MeshPoint> points) {
	//}
}