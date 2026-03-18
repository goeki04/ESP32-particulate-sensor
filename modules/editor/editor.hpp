#pragma once
#include "a_ISubsystem.hpp"
#include "gui_renderer.h"
#include "scene.hpp"
#include "a_math.hpp"
namespace Andromeda {
	class Renderer;
	class Editor : public ISubsystem{
		Renderer* m_Renderer = nullptr;
		Scene* m_Scene = nullptr;
		amath::CameraData m_EditorCamData;
		Gui::GuiRenderer m_GuiRenderer;
	public:
		void start() override;
		void update() override;
		void destroy() override;

		void updateEditorCameraRay();
		bool RayIntersectsXZPlane(const amath::Ray& ray, float planeY, vec3& outHitPoint);
		void cameraMovement(amath::CameraData& camdata);
		vec3 getCameraPos(const amath::CameraData& cam) const;
		mat4 getViewMatrix(const amath::CameraData& cam) const;
		void updatePickingRay(amath::CameraData& cam);
		void zoom(amath::CameraData& cam, SDL_Event* event);
		void setProjectionMatrix(amath::CameraData& cam, float viewPortSizeX, float viewportSizeY) const;
		static mat4 getProjectionMatrix(const amath::CameraData& cam);
		mat4 calculateCameraOrbit(amath::CameraData& cam);
		amath::Ray cursorToWorldRay(const amath::CameraData& cam) const;
	};
}