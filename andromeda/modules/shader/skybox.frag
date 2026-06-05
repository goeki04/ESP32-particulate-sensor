#version 460 core

layout (location = 0) out vec4 FragColor;
layout (location = 0) in vec3 localPos;

layout (binding = 0) uniform samplerCube environmentMap;

void main(){
    vec3 envColor = texture(environmentMap, localPos).rgb;
    envColor *= 0.5;

    FragColor = vec4(envColor, 1.0);
}