#version 330 core
in vec2 UV;

uniform bool flip;
uniform sampler2D albedo;

out vec4 outColor;
void main() {
	vec2 uvs = UV;
	if (flip) uvs.x = 1 - uvs.x;
	outColor = texture(albedo, uvs);
}