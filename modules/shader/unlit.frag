#version 460 core

layout(location = 0) out vec4 FragColor;

in vec3 normal;
in vec2 texCoords;
in vec3 color;

struct DirLight {
    vec3 color;
    vec3 direction;
};
uniform DirLight sunLight;
uniform vec3 ambientLight;
layout(binding = 0) uniform sampler2D textureSampler;

void main() {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-sunLight.direction);
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffcolor = diff * sunLight.color;
    vec3 light = ambientLight + diffcolor;
    
    vec4 texColor = texture(textureSampler, texCoords);
    vec3 finalColor = light * color * texColor.rgb;

    vec3 gammaCorrected = pow(finalColor, vec3(1.0 / 2.2));
    
    FragColor = vec4(gammaCorrected, texColor.a);
}