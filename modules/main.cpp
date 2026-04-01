#ifndef SDL_MAIN_USE_CALLBACKS
#define SDL_MAIN_USE_CALLBACKS 1
#endif
#include <SDL3/SDL_main.h>
#include "subsystem_manager.h"
#include "window_manager.h"
#include "input/input_manager.hpp"
#include "resource/resource_manager.h"
#include "renderer/renderer.h"
#include "core/a_event_manager.hpp"
#include "network/esphome_client.h"
#include "editor/editor.hpp"
#include "scene/scene.hpp"
Andromeda::Window::WindowManager windowManager;
Andromeda::InputSystem inputManager;
Andromeda::Renderer renderer;
Andromeda::ResourceManager resourceManager;
Andromeda::Editor editor;
Andromeda::SceneManager sceneManager;
static Andromeda::Network::ESPHomeClient g_EspClient;

SDL_AppResult SDL_Init() {
    SDL_SetAppMetadata("ESP32", "1.0", "ESP32.goeki.com");
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_Init();
    Andromeda::SystemManager::getInstance().addSubsystem(&windowManager);
    Andromeda::SystemManager::getInstance().addSubsystem(&inputManager);
    Andromeda::SystemManager::getInstance().addSubsystem(&resourceManager);
    Andromeda::SystemManager::getInstance().addSubsystem(&sceneManager);
    Andromeda::SystemManager::getInstance().addSubsystem(&renderer);
    Andromeda::SystemManager::getInstance().addSubsystem(&editor);
    Andromeda::SystemManager::getInstance().startSubsystems();

    g_EspClient.getDecoder().addOnMessageCallback([](uint32_t type, const std::vector<uint8_t>& payload) {
        SDL_Log("MESSAGE EMPFANGEN! Typ: %u, Bytes: %zu", type, payload.size());
        });
    SDL_Log("Starte Verbindung zum ESP32...");
    g_EspClient.connect("192.168.178.92", "6053");
    return SDL_APP_CONTINUE;
}
  
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS; 
    }
    Andromeda::SystemManager::getInstance().updateEvent(event);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    Andromeda::SystemManager::getInstance().updateSubsystems();
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    Andromeda::SystemManager::getInstance().destroy();
}