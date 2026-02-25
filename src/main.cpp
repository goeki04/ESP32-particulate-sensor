#include "pch.h"
#include "SubsystemManager.h"
#include "WindowManager.h"
#include "ResourceManager.h"
#include "Registry.h"
#include "Renderer.h"
#include "camera.h"
#include "ESPHomeClient.h"
Window::WindowManager windowManager;
Renderer renderer;
ResourceManager resourceManager;
ECS::ComponentRegistry componentManager;
static ESPHomeClient g_EspClient;
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
    SystemManager::getInstance().addSubsystem(&windowManager);
    SystemManager::getInstance().addSubsystem(&resourceManager);
    SystemManager::getInstance().addSubsystem(&componentManager);
    SystemManager::getInstance().addSubsystem(&renderer);
    SystemManager::getInstance().startSubsystems();

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
    SystemManager::getInstance().updateEvent(event);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    SystemManager::getInstance().updateSubsystems();
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    SystemManager::getInstance().destroy();
}