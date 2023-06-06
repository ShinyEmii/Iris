#version 330 core
in vec2 UV;

uniform bool flip;
uniform vec2 spriteSize;
uniform vec2 spriteOffset;

uniform sampler2D albedo;

out vec4 outColor;
void main() {
	vec2 uvs = UV;
	if (flip) uvs.x = 1 - uvs.x;
	outColor = texture(albedo, uvs * spriteSize + spriteOffset * 1 * spriteSize);
}