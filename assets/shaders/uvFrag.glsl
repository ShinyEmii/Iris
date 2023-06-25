#version 330 core
in vec2 UV;

out vec4 outColor;
void main() {
	outColor = vec4(vec3(UV, 0.0f), 1.0f);
}