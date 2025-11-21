#include "pch.h"
#include "SubsystemManager.h"
#include "WindowManager.h"
#include "GuiManager.h"
GuiManager guiManager;
WindowManager windowManager;

SDL_AppResult SDL_Init() {
    SDL_SetAppMetadata("ESP32", "1.0", "ESP32.bosch.com");
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
    SystemManager::getInstance().addSubsystem(&guiManager);
    SystemManager::getInstance().startSubsystems();
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
    SDL_DestroyRenderer(windowManager.m_Renderer);
    SDL_DestroyWindow(windowManager.m_Window);
    SDL_Quit();
}