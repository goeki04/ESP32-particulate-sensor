#pragma once

/**
 * @file a_particle.hpp
 * @brief GPU-driven particle system data types (particle payload and emitter).
 */

#include "a_Primitives.hpp"
#include "a_rhi_types.hpp"
#include <variant>
#include "a_rhi_storage_buffer.hpp"
#include "a_IGraphicsContext.hpp"
#include "a_shader_generated_compute.hpp"
#include "a_math.hpp"
namespace Andromeda {

	struct SphereShape {
		float radius = 1.0f; ///< Radius of the sphere shape.
	};

	struct HemisphereShape {
		float radius = 1.0f; ///< Radius of the hemisphere shape.
	};

	struct ConeShape {
		float radius = 1.0f; ///< Radius of the base of the cone.
		float height = 2.0f; ///< Height of the cone.
		float angle = 25.0f; ///< Angle of the cone in degrees.
	};

	struct BoxShape {
		vec3 dimensions = vec3(1.0f); ///< Dimensions of the box shape along each axis.
	};

	enum class ParticleEmitterShape {
		Sphere, ///< Emit particles from a sphere surface.
		Cone,   ///< Emit particles from a cone surface.
		Cube,   ///< Emit particles from a cube volume.
		Hemisphere ///< Emit particles from a hemisphere surface.
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
		std::variant<SphereShape, HemisphereShape, ConeShape, BoxShape> shape; ///< Shape of the emitter.
		/** @brief Allocates the GPU storage buffer and loads/links the compute and render shaders. */
		void initialize(IGraphicsContext& context, ShaderProgramHandle computeShader, ShaderProgramHandle renderShader, amath::CameraData& data);

		/**
		 * @brief Advances the simulation by dispatching the compute shader for this frame.
		 * @param deltaTime Elapsed time since the last update, in seconds.
		 */
		void update();

		/** @brief Issues the draw call that renders the current particle state. */
		void render();
	private:
		bool m_IsInitialized = false; ///< Tracks whether the emitter has been initialized.
		ShaderProgramHandle m_ComputeShader; ///< Compute shader that updates particle positions/velocities.
		ShaderProgramHandle m_RenderShader;  ///< Shader program used to render the particles.
		RHIStorageBuffer m_ParticleBuffer; ///< GPU storage buffer that holds the particle pool.
		RHIConstantBuffer m_EmitterSettingsBuffer; ///< Constant buffer that holds the emitter settings.
		RHIConstantBuffer m_CameraDataBuffer; ///< Constant buffer that holds the camera data.
		amath::CameraData* m_SceneCamera; ///< Pointer to the camera data used for rendering.
		Generated::Compute::EmitterSettings m_EmitterSettings; ///< CPU-side mirror of the `EmitterSettings` UBO; edited here and re-uploaded to @c m_EmitterSettingsBuffer every update().
		IGraphicsContext* m_Context = nullptr; ///< Pointer to the graphics context for rendering.
	};

}
