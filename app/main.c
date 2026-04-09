#include "../engine/engine.h"
#include "../engine/constants.h"
#include "../renderer/renderer.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static float g_dt = 0.1f;

#ifdef __EMSCRIPTEN__
static void main_loop(void)
{
    if (!RendererPollEvents())
    {
        emscripten_cancel_main_loop();
        EngineCleanup();
        RendererShutdown();
        return;
    }

    EngineStep(g_dt);
    RendererRender(EngineGetParticles(), EngineGetParticleCount());

    RendererGuiState ui = RendererGetGuiState();
    if (ui.reset_requested)
    {
        EngineSetElasticity(ui.elasticity);
        EngineReset(ui.particle_count);
    }
}
#endif

int main(void)
{
    int   particle_count = MAX_PARTICLE;
    float elasticity     = 0.97f;

    EngineInit(particle_count);
    EngineSetElasticity(elasticity);

    if (RendererInit(particle_count, elasticity) != 0)
        return 1;

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    int running = 1;

    while (running)
    {
        running = RendererPollEvents();
        EngineStep(g_dt);
        RendererRender(EngineGetParticles(), EngineGetParticleCount());

        RendererGuiState ui = RendererGetGuiState();
        if (ui.reset_requested)
        {
            EngineSetElasticity(ui.elasticity);
            EngineReset(ui.particle_count);
        }
    }

    EngineCleanup();
    RendererShutdown();
#endif
    return 0;
}
