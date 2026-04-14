#pragma once
#include "a_ISubsystem.hpp"
#include "gui_renderer.h"
#include "scene.hpp"
#include "a_math.hpp"
#include "a_EditorContext.hpp"
#include "a_SelectionContext.hpp"
namespace Andromeda {
	class Renderer;
	class Editor : public ISubsystem{
		Renderer* m_Renderer;
		SceneManager* m_SceneManager;
		Gui::GuiRenderer m_GuiRenderer;
		SelectionContext m_Selection;
		Gui::EditorContext m_EditorContext;
	public:
		Editor()
			: m_Renderer(nullptr),
			  m_SceneManager(nullptr)
		{ }

		void initEditorContext();
		void start() override;
		void update() override;
		void updateEvent(SDL_Event* event) override;
		void destroy() override;

		static bool RayIntersectAABB(const amath::CameraData& cam, const ECS::Component::AABB& aabb, const mat4& modelMatrix);
		void editorPicking(const amath::CameraData* cam);
		static constexpr std::string_view GetStaticName() { return "Editor"; }
		[[nodiscard]] const char* getSubsystemName() const override {
			return GetStaticName().data();
		}
		void updateEditorCameraRay() const;

		void cameraMovement(amath::CameraData& cameraData);
		[[nodiscard]] static vec3 getCameraPos(const amath::CameraData& cam);
		[[nodiscard]] static mat4 getViewMatrix(const amath::CameraData& cam);

		static void updatePickingRay(amath::CameraData& cam);

		static void zoom(amath::CameraData& cam, const SDL_Event* event);

		static void setProjectionMatrix(amath::CameraData& cam, float viewPortSizeX, float viewportSizeY);
		static mat4 getProjectionMatrix(const amath::CameraData& cam);

		static mat4 calculateCameraOrbit(amath::CameraData& cam);
		[[nodiscard]] static amath::Ray cursorToWorldRay(const amath::CameraData& cam);
	};
}