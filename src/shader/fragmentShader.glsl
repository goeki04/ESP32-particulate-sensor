#version 460 core
out vec4 FragColor;
in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;
struct DirLight{
    vec3 color;
    vec3 direction;
};
uniform DirLight sunLight;
uniform sampler2D texture1;
uniform vec3 ambientLight;
void main(){
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-sunLight.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffcolor = diff * sunLight.color;
    vec3 light = ambientLight + diffcolor;
    vec4 tex = texture(texture1, texCoords);
    FragColor = vec4(light * tex.rgb, tex.a);
}