#version 460 core

/**
 * @file prefilter.frag
 * @brief Prefilter Environment Map Generation
 *
 * This shader performs Importance Sampling using the GGX distribution to 
 * pre-filter an HDR environment cubemap. It is used to generate the 
 * "Prefiltered Color" used in IBL (Image Based Lighting) to calculate 
 * specular reflections for various roughness values.
 *
 * @note Implements the technique described by Brian Karis (Epic Games).
 */

layout (location = 0) out vec4 FragColor;
layout (location = 0) in vec3 localPos;

// Input Environment Map and Roughness settings
layout (binding = 0) uniform samplerCube environmentMap;
layout (std140, binding = 1) uniform roughnessBuffer {
    float roughness;
};

const float PI = 3.14159265359;

/**
 * @brief Trowbridge-Reitz GGX Normal Distribution Function.
 * @param N Surface normal.
 * @param H Half-way vector.
 * @param roughness Material roughness.
 * @return Probability density of microfacets aligned with H.
 */
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

/**
 * @brief Van der Corput sequence for low-discrepancy sampling.
 * @param bits Input integer index.
 * @return A radical inverse float value in [0, 1].
 */
float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

/**
 * @brief Generates a Hammersley point set sample.
 * @param i Sample index.
 * @param N Total number of samples.
 * @return A 2D Hammersley point.
 */
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i) / float(N), RadicalInverse_VdC(i));
} 

/**
 * @brief Generates a sample vector based on GGX distribution (Importance Sampling).
 * @param Xi Hammersley sample.
 * @param N Surface normal.
 * @param roughness Material roughness.
 * @return Normalized importance-sampled vector.
 */
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness * roughness;
    
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
    
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
} 

void main()
{
    vec3 N = normalize(localPos);    
    vec3 R = N; // Assume view direction equals normal for prefiltering
    vec3 V = R;

    const uint SAMPLE_COUNT = 1024u;
    float totalWeight = 0.0;    
    vec3 prefilteredColor = vec3(0.0);    

    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        //Generate sample direction
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0)
        {
            float D   = DistributionGGX(N, H, roughness);
            float NdotH = max(dot(N, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);
            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001; 

            float resolution = 512.0;
            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

            float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel); 
            
            prefilteredColor += textureLod(environmentMap, L, mipLevel).rgb * NdotL;
            totalWeight      += NdotL;
        }
    }
    
    prefilteredColor = prefilteredColor / totalWeight;
    FragColor = vec4(prefilteredColor, 1.0);
}