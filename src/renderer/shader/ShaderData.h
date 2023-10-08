#pragma once
#include "../../utility/ByteBuffer.h"
namespace Iris {
	namespace Renderer {
		struct ShaderData {
			bool valid;
			std::string vertexData;
			std::string fragmentData;
		};
		ShaderData createShaderDataFromBuffer(Utility::ByteBuffer& buf) {
			return { 0 };
		}
	}
}