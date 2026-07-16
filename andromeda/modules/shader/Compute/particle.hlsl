
struct Particle
{
    float4 position;
    float4 velocity;
};

RWStructuredBuffer<Particle> particles : register(u1);

/** HLSL Variables
 *  SV_DispatchThreadID: global index of the thread
 *  SV_GroupThreadID: local index of the thread within the group
 *  SV_GroupID: index of the group
 *  SV_GroupIndex: linear index of the thread within the group
 */

cbuffer EmitterSettings : register(b2)
{
    float deltaTime;
    uint maxParticles;
    float3 emitterPosition;
    int padding1;
    int padding2;
    int padding3;
};

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
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
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

    pos.y += 0.5f * deltaTime;
    remainingLife -= deltaTime;

    if (remainingLife <= 0.0f)
    {
        pos = emitterPosition;

        float finalMaxLife = (maxLife <= 0.0f) ? (1.0f + hash1(index) * 2.0f) : maxLife;
        remainingLife = finalMaxLife;

        particles[index].velocity.w = finalMaxLife;
        particles[index].velocity.xyz = float3(0.0f, 1.0f, 0.0f);
    }

    particles[index].position = float4(pos, remainingLife);
}
