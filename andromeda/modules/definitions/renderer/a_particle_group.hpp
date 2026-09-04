#pragma once
#include "a_primitives.hpp"
#include <string>
namespace Andromeda {
    struct ParticleGroup {
        u8 particleGroupID = 0;
        std::string groupName = "DefaultGroup";  ///< Name of the particle group for identification.
        u32 particleCount = 1000;                ///< Maximum number of particles allowed in this group.
        float size = 1.0f;                       ///< Size of each particle in this group.
        vec3 velocity = {1.0f, 1.0f, 1.0f};      ///< Initial velocity of particles in this group.
        vec3 particleColor = {1.0f, 1.0f, 1.0f}; ///< Color of the particles (RGB).
        float minLifeTime = 0.0f;                ///< Minimum lifetime of the particles in seconds.
    };
} // namespace Andromeda   