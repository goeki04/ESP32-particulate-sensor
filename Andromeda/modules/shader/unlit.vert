#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords; 
layout(location = 3) in vec3 aColor;

layout(binding = 0) uniform UnlitCameraBuffer {
    mat4 viewMatrix;
    mat4 projMatrix;
};

layout(binding = 1) uniform UnlitObjectBuffer {
    mat4 model;
};

layout(location = 0) out vec3 normal;
layout(location = 1) out vec2 texCoords;
layout(location = 2) out vec3 color;

void main()
{
    vec3 fragPos = vec3(model * vec4(aPos, 1.0));
    
    normal = mat3(transpose(inverse(model))) * aNormal;  
    
    gl_Position = projMatrix * viewMatrix * vec4(fragPos, 1.0);
    
    color = aColor;
    texCoords = aTexCoords;
}