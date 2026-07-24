#version 460 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) in float vLifeFade;
void main(){
    float distance = length(gl_PointCoord - vec2(0.5));
    if(distance > 0.5){
        discard;
    }

    float alpha = smoothstep(0.5,0.0, distance);
    FragColor = vec4(0.9,0.9,0.9, alpha * vLifeFade);
}