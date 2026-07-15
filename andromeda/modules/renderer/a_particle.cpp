#include "a_particle.hpp"
#include "a_logger.hpp"
#include "a_subsystem_manager.hpp"
#include "a_shader_generated.hpp"
namespace Andromeda {
	/**
	 * @brief Allocates the GPU-side particle pool and emitter settings buffer, and sets default emission parameters.
	 * @details Stores the (already loaded) compute and render shader handles, initializes
	 *          @c m_EmitterSettings with default values (origin position, 1000 max particles,
	 *          zeroed padding), then allocates the SSBO backing the particle pool
	 *          (`maxParticles * sizeof(Generated::Compute::Particle)` bytes) and the UBO
	 *          backing @c EmitterSettings.
	 * @param context The graphics context used to allocate GPU resources.
	 * @param computeShader Handle to the already-loaded particle update compute shader.
	 * @param renderShader Handle to the already-loaded particle render shader.
	 * @param data Reference to the camera data.
	 */
	void ParticleEmitter::initialize(IGraphicsContext& context, ShaderProgramHandle computeShader, ShaderProgramHandle renderShader,amath::CameraData& data) {

		m_Context = &context;
		if (m_Context == nullptr) {
			A_WARN("Graphics context is null. Cannot initialize ParticleEmitter.");
			return;
		}
		m_ComputeShader = computeShader;
		m_RenderShader = renderShader;
		m_EmitterSettings.emitterPosition = vec3(0.0f);
		m_EmitterSettings.maxParticles = 1000;
		m_EmitterSettings.padding1 = 0;
		m_EmitterSettings.padding2 = 0;
		m_EmitterSettings.padding3 = 0;
		m_IsInitialized = true;
		shape = BoxShape{ vec3(1.0f) };
		m_ParticleBuffer.create(BufferUsage::DynamicDraw,m_EmitterSettings.maxParticles  * sizeof(Generated::Compute::Particle), nullptr);
		m_ParticleBuffer.clear();
		m_EmitterSettingsBuffer.initialize(sizeof(Generated::Compute::EmitterSettings));
		m_SceneCamera = &data;
		m_CameraDataBuffer.initialize(sizeof(Generated::CameraData));
	}
	/**
	 * @brief Advances the particle simulation by dispatching the compute shader for this frame.
	 * @details No-ops if the emitter has no capacity or was never initialized. Otherwise binds
	 *          the particle SSBO (slot 1) and the @c EmitterSettings UBO (slot 2), refreshes
	 *          @c m_EmitterSettings.deltaTime from the current frame time, re-uploads it to
	 *          @c m_EmitterSettingsBuffer, and dispatches one compute work group per 256
	 *          particles (matching `layout(local_size_x = 256, ...)` in particle.comp).
	 * @param deltaTime Elapsed time since the last update, in seconds.
	 */
	void ParticleEmitter::update() {
		if (m_EmitterSettings.maxParticles == 0) return;
		if (m_IsInitialized == false) {
			A_WARN("ParticleEmitter not initialized!");
			return;
		}
		m_Context->bindShaderProgram(m_ComputeShader);
		m_ParticleBuffer.bind(1);

		m_EmitterSettingsBuffer.bind(2);
		m_EmitterSettings.deltaTime = SystemManager::s_deltaTime;
		m_EmitterSettingsBuffer.setData(&m_EmitterSettings, sizeof(Generated::Compute::EmitterSettings));


		u32 threadGroupsX = (m_EmitterSettings.maxParticles + 255) / 256;
		m_Context->dispatchCompute(m_ComputeShader, threadGroupsX, 1, 1);
	}

	/**
	 * @brief Issues the draw call that renders the current particle state.
	 * @details No-ops if the emitter was never initialized or has no capacity. Otherwise binds
	 *          the render shader and the particle SSBO (slot 1) for it to read from, then draws
	 *          one point instance per particle (vertex-less, point-primitive instanced draw).
	 */
	void ParticleEmitter::render() {
		if (!m_IsInitialized || m_EmitterSettings.maxParticles == 0)
		{
			A_WARN("ParticleEmitter not initialized or maxParticles is zero. Cannot render.");
			return;
		}
		m_Context->bindShaderProgram(m_RenderShader);
		m_ParticleBuffer.bind(1);
		m_CameraDataBuffer.bind(2);
		Generated::CameraData cameraData;
		cameraData.view = m_SceneCamera->viewMatrix;
		cameraData.proj = m_SceneCamera->projection;
		m_CameraDataBuffer.setData(&cameraData, sizeof(Generated::CameraData));
		m_Context->drawInstanced(DrawMode::Points, 1, m_EmitterSettings.maxParticles, 0);
	}
}
