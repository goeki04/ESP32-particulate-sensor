#version 460 core

layout(location = 0) in vec3 aPos;

layout(binding = 0) uniform MaskCameraBuffer {
    mat4 viewMatrix;
    mat4 projMatrix;
};

layout(binding = 1) uniform MaskObjectBuffer {
    mat4 model;
};

void main()
{
    gl_Position = projMatrix * viewMatrix * model * vec4(aPos, 1.0);
}