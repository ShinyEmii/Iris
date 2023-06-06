#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 uvs;
uniform mat4 model;
uniform mat4 proj;
uniform mat4 view;

out vec2 UV;
void main() {
    mat4 mvp = proj * view * model;
    gl_Position = mvp * vec4(vertex, 1.0f);
    UV = uvs;
}