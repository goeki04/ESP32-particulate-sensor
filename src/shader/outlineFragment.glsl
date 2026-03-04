#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D fboSampler; 
uniform sampler2D maskSampler;
uniform vec2 texelSize;

void main() {
    vec4 sceneColor = texture(fboSampler, TexCoords);
    float mask = texture(maskSampler, TexCoords).r;
    if (mask > 0.5) {
        FragColor = sceneColor;
        return;
    }
    float neighborMask = 0.0;
    int radius = 2;
    // . . . . .
    // . . . . .
    // . . x . .
    // . . . . .
    // . . . . .
    //-2   0   2
    for(int x = -radius; x <= radius; x++) {
        for(int y = -radius; y <= radius; y++) {
            neighborMask += texture(maskSampler, TexCoords + vec2(x, y) * texelSize).r;
        }
    }
    if (neighborMask > 0.01) {
        FragColor = vec4(1.0, 0.5, 0.0, 1.0);
    } else {
        FragColor = sceneColor;
    }
}