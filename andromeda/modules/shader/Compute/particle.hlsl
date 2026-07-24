
struct Particle
{
    float4 position; //xyz = Position, w = remaining lifetime in seconds
    float4 velocity; //xyz = Velocity, w = max lifetime in seconds
};

RWStructuredBuffer<Particle> particles : register(u1);

/** HLSL Variables
 *  SV_DispatchThreadID: global index of the thread
 *  SV_GroupThreadID: local index of the thread within the group
 *  SV_GroupID: index of the group
 *  SV_GroupIndex: linear index of the thread within the group
 */

///Constant buffers are 16 bit aligned in memory in glsl as well as in hlsl. 
//So we need to make sure that the size of the struct is a multiple of 16 bytes.
cbuffer EmitterSettings : register(b2)
{
    float deltaTime; // 0-3
    uint maxParticles; // 4-7
    float sphereRadius; // 8-11   
    float coneHeight; // 12-15
    float3 emitterPosition; // 16-27
    float coneAngle; // 28-31
    float3 boxDimensions; // 32-43
    float emissionRate; // 44-47
    uint activeParticleCount; // 48-51
    float cylinderRadius; // 52-55
    float cylinderHeight; // 56-59
    float gravity; // 60-63
    float drag; // 64-67
}
uint pcg_hash(uint inputValue)
{
    uint state = inputValue * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float hash1(uint seed)
{
    return float(pcg_hash(seed)) * (1.0f / 4294967295.0f);
}

float3 hash3(uint seed)
{
    uint x = pcg_hash(seed);
    uint y = pcg_hash(x);
    uint z = pcg_hash(y);
    return float3(x, y, z) * (1.0f / 4294967295.0f);
}

// Single entry point (matches the fixed "-E main" DXC invocation in
// CompileShaders.cmake). Combines the former RunInit/RunUpdate kernels:
// a particle buffer starts zero-initialized, so every slot is already
// "dead" (remainingLife <= 0) on the first dispatch and falls into the
// same respawn branch that later revivals also use - no separate init
// pass required. The randomized start-life formula that RunInit used is
// preserved inside that respawn branch via hash1(index).
[numthreads(256, 1, 1)]
void BoxParticle(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint index = dispatchThreadId.x;
    if (index >= maxParticles)
    {
        return;
    }

    float3 pos = particles[index].position.xyz;
    float3 vel = particles[index].velocity.xyz;
    float maxLife = particles[index].velocity.w;
    float remainingLife = particles[index].position.w;
    pos += vel * deltaTime;
    remainingLife -= deltaTime;

    if (remainingLife <= 0.0f && index < activeParticleCount)
    {
        float3 random = hash3(index);
        float2 offset = (random.xz * 2.0f - 1.0f) * boxDimensions.xz * 0.5f;
        pos = emitterPosition + float3(offset.x, 0.0f, offset.y);

        float finalMaxLife = (maxLife <= 0.0f) ? (1.0f + random.y * 2.0f) : maxLife;
        remainingLife = finalMaxLife;

        particles[index].velocity.w = finalMaxLife;
        particles[index].velocity.xyz = float3(0.0f, 1.0f, 0.0f);
    }

    particles[index].position = float4(pos, remainingLife);
}

// Uniform sampling on the sphere surface (Y-up pole axis): theta = azimuth
// around Y, y = cos(polar angle) remapped to [-1,1] so the whole sphere is
// covered. saturate() guards the sqrt against tiny negative values from
// floating-point rounding when y is close to +-1.
[numthreads(256, 1, 1)]
void SphereParticle(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint index = dispatchThreadId.x;
    if (index >= maxParticles)
    {
        return;
    }

    float3 pos = particles[index].position.xyz;
    float3 vel = particles[index].velocity.xyz;
    float maxLife = particles[index].velocity.w;
    float remainingLife = particles[index].position.w;
    pos += vel * deltaTime;
    remainingLife -= deltaTime;

    if (remainingLife <= 0.0f && index < activeParticleCount)
    {
        float3 random = hash3(index);
        float theta = random.x * 6.2831853f;
        float y = random.y * 2.0f - 1.0f;
        float radiusXZ = sqrt(saturate(1.0f - y * y));
        float3 dir = float3(radiusXZ * cos(theta), y, radiusXZ * sin(theta));
        pos = emitterPosition + dir * sphereRadius;

        float finalMaxLife = (maxLife <= 0.0f) ? (1.0f + hash1(index ^ 0x9E3779B9u) * 2.0f) : maxLife;
        remainingLife = finalMaxLife;

        particles[index].velocity.w = finalMaxLife;
        particles[index].velocity.xyz = dir; 
    }

    particles[index].position = float4(pos, remainingLife);
}

// Point inside a cone along +Y: height picks a random slice along the
// axis, radius at that slice is bounded by tan(coneAngle)*height, and
// sqrt(random.z) keeps the in-disc sampling area-uniform (not just
// edge-biased) for that slice.
[numthreads(256, 1, 1)]
void ConeParticle(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint index = dispatchThreadId.x;
    if (index >= maxParticles)
    {
        return;
    }

    float3 pos = particles[index].position.xyz;
    float3 vel = particles[index].velocity.xyz;
    float maxLife = particles[index].velocity.w;
    float remainingLife = particles[index].position.w;
    pos += vel * deltaTime;
    remainingLife -= deltaTime;

    if (remainingLife <= 0.0f && index < activeParticleCount)
    {
        float3 random = hash3(index);
        float height = random.x * coneHeight;
        float angleRad = coneAngle * 0.0174533f; // Degrees -> Radiant
        float maxRadiusAtHeight = tan(angleRad) * height;
        float theta = random.y * 6.2831853f;
        float radius = sqrt(random.z) * maxRadiusAtHeight;
        float3 localOffset = float3(radius * cos(theta), height, radius * sin(theta));
        pos = emitterPosition + localOffset;

        float finalMaxLife = (maxLife <= 0.0f) ? (1.0f + hash1(index ^ 0x9E3779B9u) * 2.0f) : maxLife;
        remainingLife = finalMaxLife;

        particles[index].velocity.w = finalMaxLife;
        particles[index].velocity.xyz = float3(0.0f, 1.0f, 0.0f);
    }

    particles[index].position = float4(pos, remainingLife);
}

// Point inside a cylinder along +Y: height is uniform along the axis,
// radius is area-uniform across the circular cross-section (sqrt(random.z)
// avoids clustering samples near the center, same trick as ConeParticle).
[numthreads(256, 1, 1)]
void CylinderParticle(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint index = dispatchThreadId.x;
    if (index >= maxParticles)
    {
        return;
    }

    float3 pos = particles[index].position.xyz;
    float3 vel = particles[index].velocity.xyz;
    float maxLife = particles[index].velocity.w;
    float remainingLife = particles[index].position.w;
    
    vel += float3(0.0f, -gravity, 0.0f) * deltaTime;
    vel *= pow(drag, deltaTime);
    pos += vel * deltaTime;
    remainingLife -= deltaTime;

    if (remainingLife <= 0.0f && index < activeParticleCount)
    {
        float3 random = hash3(index);
        float height = random.x * cylinderHeight;
        float theta = random.y * 6.2831853f;
        float radius = sqrt(random.z) * cylinderRadius;
        float3 localOffset = float3(radius * cos(theta), height, radius * sin(theta));
        pos = emitterPosition + localOffset;

        float finalMaxLife = (maxLife <= 0.0f) ? (1.0f + hash1(index ^ 0x9E3779B9u) * 2.0f) : maxLife;
        remainingLife = finalMaxLife;
        vel = float3(0.0f, 0.0f, 0.0f);
        particles[index].velocity.w = finalMaxLife;

    }
    particles[index].velocity.xyz = vel;
    particles[index].position = float4(pos, remainingLife);
}

[numthreads(256, 1, 1)]
void HemisphereParticle(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    uint index = dispatchThreadId.x;
    if (index >= maxParticles)
    {
        return;
    }

    float3 pos = particles[index].position.xyz;
    float3 vel = particles[index].velocity.xyz;
    float maxLife = particles[index].velocity.w;
    float remainingLife = particles[index].position.w;
    pos += vel * deltaTime;
    remainingLife -= deltaTime;

    if (remainingLife <= 0.0f && index < activeParticleCount)
    {
        float3 random = hash3(index);
        float theta = random.x * 6.2831853f;
        float y = random.y;
        float radiusXZ = sqrt(saturate(1.0f - y * y));
        float3 dir = float3(radiusXZ * cos(theta), y, radiusXZ * sin(theta));
        pos = emitterPosition + dir * sphereRadius;

        float finalMaxLife = (maxLife <= 0.0f) ? (1.0f + hash1(index ^ 0x9E3779B9u) * 2.0f) : maxLife;
        remainingLife = finalMaxLife;

        particles[index].velocity.w = finalMaxLife;
        particles[index].velocity.xyz = dir;
    }

    particles[index].position = float4(pos, remainingLife);
}
