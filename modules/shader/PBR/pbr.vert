#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexcoords;
layout (location = 2) in vec3 aNormal;

layout (location = 0) out vec2 TexCoords;
layout (location = 1) out vec3 WorldPos;
layout (location = 2) out vec3 Normal;

layout (std140,binding = 0) uniform pbrBuffer{
     mat4 model;
     mat4 view;
     mat4 projection;
};
void main(){
    TexCoords = aTexcoords;

    WorldPos = vec3(model*vec4(aPos,1.0));

    Normal = mat3(transpose(inverse(model))) * aNormal;

    gl_Position = projection * view * vec4(WorldPos, 1.0);
}