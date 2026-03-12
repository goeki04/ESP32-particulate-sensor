#pragma once
#include "subsystem.h"
#include "gui_renderer.h"
#include "scene.hpp"
#include "a_math.hpp"
namespace Andromeda {
	class Renderer;
	class Editor : public ISubsystem{
		Renderer* m_Renderer = nullptr;
		Scene* m_Scene = nullptr;
		amath::Camera m_EditorCam;
		Gui::GuiRenderer m_GuiRenderer;

		void start() override;
		void update() override;
		void destroy() override;

		bool RayIntersectsXZPlane(const amath::Ray& ray, float planeY, vec3& outHitPoint);
		void cameraMovement();
		vec3 getCameraPos() const;
		mat4 getViewMatrix() const;
		void updatePickingRay();
		void zoom(SDL_Event* event);
		void setProjectionMatrix(float viewPortSizeX, float viewportSizeY) const;
		static mat4 getProjectionMatrix();
		mat4 calculateCameraOrbit();
		amath::Ray cursorToWorldRay() const;
	};
}