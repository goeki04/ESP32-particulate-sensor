#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 0) out vec3 localPos;

layout(location = 0) uniform mat4 proj;
layout(location = 4) uniform mat4 view;

void main() {
    localPos = aPos;
    gl_Position = proj * view * vec4(localPos, 1.0);
}