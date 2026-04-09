#include "gui.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

static GuiState      g_state;
static SDL_Renderer *g_renderer = nullptr;

void GuiInit(SDL_Window *window, SDL_Renderer *renderer, int initial_particle_count, float initial_elasticity)
{
    g_renderer              = renderer;
    g_state.reset_requested = 0;
    g_state.particle_count  = initial_particle_count;
    g_state.elasticity      = initial_elasticity;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr; /* no imgui.ini persistence */

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
}

void GuiShutdown(void)
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void GuiHandleEvent(SDL_Event *event)
{
    ImGui_ImplSDL2_ProcessEvent(event);
}

void GuiNewFrame(void)
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    g_state.reset_requested = 0;

    ImGui::SetNextWindowPos(ImVec2(10, 40), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(280, 140), ImGuiCond_Once);
    ImGui::Begin("Simulation Controls");

    if (ImGui::Button("Reset"))
        g_state.reset_requested = 1;

    ImGui::SliderInt("Particles", &g_state.particle_count, 10, 5000);
    ImGui::SliderFloat("Elasticity", &g_state.elasticity, 0.0f, 1.0f);

    if (ImGui::Button("Apply Settings"))
        g_state.reset_requested = 1;

    ImGui::End();
}

void GuiRender(void)
{
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), g_renderer);
}

GuiState GuiGetState(void)
{
    return g_state;
}
