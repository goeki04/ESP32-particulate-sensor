#version 460 core

struct Particle{
    vec4 position; //xyz = position, w = remaining lifetime
    vec4 velocity; //xyz = velocity, w = max lifetime
};

layout (std430, binding = 1) buffer particleBuffer{
    Particle particles[];
};

layout (std140, binding = 2) uniform CameraData{
    mat4 proj;
    mat4 view;
};

void main(){
    uint index = gl_InstanceID;

    vec3 worldPos = particles[index].position.xyz;
    float remainingLife = particles[index].position.w;

    if(remainingLife <= 0.0){
        gl_Position = vec4(0.0,0.0,0.0,0.0);
        gl_PointSize = 0.0;
        return;
    }
    gl_Position = proj * view * vec4(worldPos, 1.0);
    gl_PointSize = 20.0;
}