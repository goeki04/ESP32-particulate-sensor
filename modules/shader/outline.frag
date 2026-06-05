#version 460 core

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec2 TexCoords;

layout(binding = 10) uniform sampler2D fboSampler;
layout(binding = 11) uniform sampler2D maskSampler;

layout(binding = 0) uniform OutlineParamsBuffer {
    vec2 texelSize;
};

vec3 ACESFilm(vec3 x) {
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
    vec4 sceneColor = texture(fboSampler, TexCoords);
    float mask = texture(maskSampler, TexCoords).r;
    

    vec3 finalDisplayColor = sceneColor.rgb;

    if (mask < 0.5) {
        float radius = 2.0; 
        float neighborMask = 0.0;
        
        neighborMask += texture(maskSampler, TexCoords + vec2(radius, 0.0) * texelSize).r;
        neighborMask += texture(maskSampler, TexCoords + vec2(-radius, 0.0) * texelSize).r;
        neighborMask += texture(maskSampler, TexCoords + vec2(0.0, radius) * texelSize).r;
        neighborMask += texture(maskSampler, TexCoords + vec2(0.0, -radius) * texelSize).r;

        if (neighborMask > 0.01) {
            finalDisplayColor = vec3(1.0, 0.5, 0.0);
        }
    }

    finalDisplayColor = ACESFilm(finalDisplayColor);
    finalDisplayColor = pow(finalDisplayColor, vec3(1.0 / 2.2));

    FragColor = vec4(finalDisplayColor, 1.0);
}