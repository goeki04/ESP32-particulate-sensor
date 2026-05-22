#version 460 core

layout (location = 0) out vec4 FragColor;

layout (binding = 2) uniform ColorBuffer {
    vec3 aColor;
};

void main(){
    FragColor = vec4(aColor, 1.0);
}