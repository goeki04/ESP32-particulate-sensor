#include "Renderer.h"

void Renderer::draw()
{
    SDL_SetRenderDrawColorFloat(m_SDLRenderer, 0.5, 0.5, 1.0, SDL_ALPHA_OPAQUE_FLOAT);
    SDL_RenderClear(m_SDLRenderer);
    RenderImGui();
    //ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_Renderer);
    SDL_RenderPresent(m_SDLRenderer);
}
