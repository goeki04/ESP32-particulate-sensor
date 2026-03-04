#version 460 core
out vec4 FragColor;
in vec3 worldPos;

uniform float gGridMinPixelsBetweenCells = 2.0;
uniform float gGridCellSize = 0.025;
uniform vec4 gGridColorThin  = vec4(0.5,0.5,0.5,1.0);
uniform vec4 gGridColorThick = vec4(0.0,0.0,0.0,1.0);
uniform vec3 camPos;
uniform float gGridSize = 100.0;

vec3 xAxisColor = vec3(1.0,0.0,0.0);
vec3 zAxisColor = vec3(0.0,0.0,1.0);

float satf(float x) 
{ 
    return clamp(x, 0.0, 1.0); 
}
float log10f(float x) 
{ 
    return log(max(x, 1e-6)) / log(10.0);
}

void main(){
    vec2 dvx = vec2(dFdx(worldPos.x), dFdy(worldPos.x));
    vec2 dvy = vec2(dFdx(worldPos.z), dFdy(worldPos.z));

    float lx = length(dvx);
    float ly = length(dvy);

    vec2 dudv = vec2(lx, ly);
    float l = length(dudv);
    float LOD = max(0.0,log10f(l * gGridMinPixelsBetweenCells / gGridCellSize) + 1.0);
    float gridCellSizeLOD0 = gGridCellSize * pow(10.0,floor(LOD));
    float gridCellSizeLOD1 = gridCellSizeLOD0 * 10.0;
    float gridCellSizeLOD2 = gridCellSizeLOD1 * 10.0;

    dudv = max(dudv * 4.0, vec2(1e-6));

    vec2 mod_div_dudv = mod(worldPos.xz, gridCellSizeLOD0) / dudv;
    vec2 a = vec2(1.0) - abs(clamp(mod_div_dudv, vec2(0.0), vec2(1.0)) * 2.0 - vec2(1.0));
    float Lod0a = max(a.x, a.y);

    mod_div_dudv = mod(worldPos.xz, gridCellSizeLOD1) / dudv;
    vec2 b = vec2(1.0) - abs(clamp(mod_div_dudv, vec2(0.0), vec2(1.0)) * 2.0 - vec2(1.0));
    float Lod1a = max(b.x, b.y);

    mod_div_dudv = mod(worldPos.xz, gridCellSizeLOD2) / dudv;
    vec2 c = vec2(1.0) - abs(clamp(mod_div_dudv, vec2(0.0), vec2(1.0)) * 2.0 - vec2(1.0));
    float Lod2a = max(c.x, c.y);

    float LOD_fade = fract(LOD);

    float xLine = 1.0 - satf(abs(worldPos.x) / dudv.x);
    float zLine = 1.0 - satf(abs(worldPos.z) / dudv.y);

    vec4 color;
    if(Lod2a > 0.0){
        color = gGridColorThick;
        color.a *= Lod2a;
    }
    else{
        if(Lod1a > 0.0){
            color = mix(gGridColorThick, gGridColorThin, LOD_fade);
            color.a *= Lod1a;
        }
        else{
            color = gGridColorThin;
            color.a *= (Lod0a * (1.0 - LOD_fade));
        }
    }

    if(xLine > 0.0) {
        color.rgb = mix(color.rgb, zAxisColor, xLine);
        color.a = max(color.a, xLine);
    }
    if(zLine > 0.0) {
        color.rgb = mix(color.rgb, xAxisColor, zLine);
        color.a = max(color.a, zLine);
    }

    float opacityFallof = (1.0 - satf(length(worldPos.xz - camPos.xz) / gGridSize));
    color.a *= opacityFallof;

    FragColor = color;
}