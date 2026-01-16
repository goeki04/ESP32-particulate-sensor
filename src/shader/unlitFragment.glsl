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
void main(){
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-sunLight.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffcolor = diff * sunLight.color;
    vec3 light = ambientLight + diffcolor;
    FragColor = vec4(light * color, 1.0);
}