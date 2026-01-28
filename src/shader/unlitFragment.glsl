#version 460 core
out vec4 FragColor;
in vec3 normal;
in vec3 color;
struct DirLight{
    vec3 color;
    vec3 direction;
};
uniform DirLight sunLight;
uniform vec3 ambientLight;
void main() {
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-sunLight.direction);
    
    // 1. Diffuses Licht berechnen
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffcolor = diff * sunLight.color;
    
    // 2. Gesamtes Licht berechnen
    vec3 light = ambientLight + diffcolor;
    
    // 3. Farbe im linearen Raum berechnen
    vec3 linearColor = light * color;
    
    // 4. GAMMA-KORREKTUR (Das macht den Unterschied)
    // Wir heben die dunklen Werte an, damit sie auf dem Monitor richtig erscheinen.
    // 1.0 / 2.2 entspricht ca. 0.4545
    vec3 gammaCorrected = pow(linearColor, vec3(1.0 / 2.2));
    
    FragColor = vec4(gammaCorrected, 1.0);
}