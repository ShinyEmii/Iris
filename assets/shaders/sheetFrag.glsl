#version 330 core
in vec2 UV;

uniform bool flip;
uniform vec2 spriteSize;
uniform vec2 spriteOffset;

uniform sampler2D albedo;

out vec4 outColor;
void main() {
	vec2 uv = UV;
	if (flip) uv.x = 1 - uv.x;
	outColor = texture(
		albedo, vec2(
			(uv.x / spriteSize.x) + spriteOffset.x * (1.0 / spriteSize.x),
			(uv.y / spriteSize.y) + spriteOffset.y * (1.0 / spriteSize.y)
		)
	);
}