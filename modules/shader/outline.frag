#version 460 core

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec2 TexCoords;

layout(binding = 10) uniform sampler2D fboSampler;
layout(binding = 11) uniform sampler2D maskSampler;

layout(binding = 0) uniform OutlineParamsBuffer {
    vec2 texelSize;
};

void main() {
    vec4 sceneColor = texture(fboSampler, TexCoords);
    float mask = texture(maskSampler, TexCoords).r;
    
    if (mask > 0.5) {
        FragColor = sceneColor;
        return;
    }
    
    float radius = 2.0; 
    float neighborMask = 0.0;
    
    neighborMask += texture(maskSampler, TexCoords + vec2(radius, 0.0) * texelSize).r;
    neighborMask += texture(maskSampler, TexCoords + vec2(-radius, 0.0) * texelSize).r;
    neighborMask += texture(maskSampler, TexCoords + vec2(0.0, radius) * texelSize).r;
    neighborMask += texture(maskSampler, TexCoords + vec2(0.0, -radius) * texelSize).r;

    if (neighborMask > 0.01) {
        FragColor = vec4(1.0, 0.5, 0.0, 1.0); // Orange Outline
    } else {
        FragColor = sceneColor;
    }
}