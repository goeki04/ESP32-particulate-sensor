#version 460 core
uniform mat4 view;
const vec3 pos[4] = vec3[4](
    vec3(-1.0,0.0,-1.0),
    vec3(1.0,0.0,-1.0),
    vec3(1.0,0.0,1.0),
    vec3(-1.0,0.0,1.0)
);

const int indices[6] = int[6](0,2,1,2,0,3);
void main(){
    int index = indices[gl_VertexID];
    vec4 vPos = vec4(pos[index],1.0);
    gl_Position = view * vPos;
}