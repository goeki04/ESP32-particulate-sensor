#pragma once

/**
 * @file a_particle.hpp
 * @brief GPU-driven particle system data types (particle payload and emitter).
 */

#include "a_Primitives.hpp"
#include "a_rhi_types.hpp"
namespace Andromeda {

	/**
	 * @struct Particle
	 * @brief State of a single particle as stored in the GPU shader-storage buffer.
	 *
	 * @details Updated by the compute shader each frame. @c alignas(16) keeps the layout
	 *          compatible with the std430 buffer alignment rules expected on the GPU side,
	 *          avoiding a CPU/GPU struct-size mismatch.
	 */
	struct alignas(16) Particle {
		vec3 position; ///< World-space position of the particle.
		vec3 velocity; ///< Current velocity, integrated against external forces (e.g. wind) on the GPU.
	};

	/**
	 * @struct ParticleEmitter
	 * @brief Owns and drives a pool of particles via a compute (update) and a render shader.
	 *
	 * @details Holds the emission parameters and the two shader programs that make up the
	 *          GPU pipeline: a compute shader that integrates particle motion in a storage
	 *          buffer, and a render shader that draws the resulting particles.
	 */
	struct ParticleEmitter {
	public:
		vec3 position;      ///< Spawn origin of newly emitted particles, in world space.
		u32 emissionRate;   ///< Number of particles emitted per second.
		u32 maxParticles;   ///< Capacity of the particle pool / storage buffer.
		float lifetime;     ///< Lifespan of each particle, in seconds.

		/** @brief Allocates the GPU storage buffer and loads/links the compute and render shaders. */
		void initialize();

		/**
		 * @brief Advances the simulation by dispatching the compute shader for this frame.
		 * @param deltaTime Elapsed time since the last update, in seconds.
		 */
		void update(float deltaTime);

		/** @brief Issues the draw call that renders the current particle state. */
		void render();
	private:
		ShaderProgramHandle m_ComputeShader; ///< Compute shader that updates particle positions/velocities.
		ShaderProgramHandle m_RenderShader;  ///< Shader program used to render the particles.
	};

}