#version 460 core
layout (location = 0) in vec3 aPos;

layout (location = 0) out vec3 localPos;

layout (binding = 0) uniform SkyboxBuffer {
    mat4 view;
    mat4 proj;
};

void main(){
    localPos = aPos;
    mat4 rotView = mat4(mat3(view));
    vec4 clipPos = proj * rotView * vec4(localPos, 1.0);

    gl_Position = clipPos.xyww;
}