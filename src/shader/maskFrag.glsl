#version 460 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D fboSampler; 
uniform sampler2D maskSampler;
uniform vec2 texelSize;
void main(){
    vec4 sceneColor = texture(fboSampler, TexCoords);
    float mask = texture(maskSampler, TexCoords).r;
    FragColor = vec4(mask,mask,mask,1.0);
}