#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aColor;

uniform mat4 model;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

out vec3 normal;
out vec3 fragPos;
out vec2 texCoords;
out vec3 color;
void main()
{
    fragPos = vec3(model * vec4(aPos, 1.0));
    normal = mat3(transpose(inverse(model))) * aNormal;  
    gl_Position = projMatrix * viewMatrix * vec4(fragPos,1.0);
    texCoords = aTexCoords;
    color = aColor;
}