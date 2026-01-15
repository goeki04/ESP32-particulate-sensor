#version 460 core
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec3 camPos;
uniform float gGridSize = 100.0;
const vec3 pos[4] = vec3[4](
    vec3(-1.0,0.0,-1.0),
    vec3(1.0,0.0,-1.0),
    vec3(1.0,0.0,1.0),
    vec3(-1.0,0.0,1.0)
);
out vec3 worldPos;
const int indices[6] = int[6](0,2,1,2,0,3);
void main(){
    int index = indices[gl_VertexID];
    vec3 vPos3 = pos[index] * gGridSize;
    vPos3.x += camPos.x;
    vPos3.z += camPos.z;
    vec4 vPos4 = vec4(vPos3,1.0);
    gl_Position = projMatrix * viewMatrix * vPos4;
    worldPos = vPos3;
}