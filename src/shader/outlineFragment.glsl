#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D fboSampler;  // Slot 0: Das normale Bild
uniform sampler2D maskSampler; // Slot 1: Die weiﬂe Maske
uniform vec2 texelSize;

void main() {
    vec4 sceneColor = texture(fboSampler, TexCoords);
    float mask = texture(maskSampler, TexCoords).r;

    // Wenn das Pixel zum Objekt gehˆrt, zeichnen wir das normale Bild
    if (mask > 0.5) {
        FragColor = sceneColor;
        return;
    }

    // Suche in der Nachbarschaft nach weiﬂen Pixeln der Maske
    float neighborMask = 0.0;
    int radius = 2; // Dicke der Outline (in Pixeln)

    for(int x = -radius; x <= radius; x++) {
        for(int y = -radius; y <= radius; y++) {
            neighborMask += texture(maskSampler, TexCoords + vec2(x, y) * texelSize).r;
        }
    }

    // Wenn wir am Rand eines weiﬂen Bereichs sind
    if (neighborMask > 0.01) {
        FragColor = vec4(1.0, 0.5, 0.0, 1.0); // Unity Orange
    } else {
        // Sonst einfach das Originalbild
        FragColor = sceneColor;
    }
}