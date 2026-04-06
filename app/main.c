    // Phase 1: Insert all particles into gri0
#include "../engine/engine.h"
#include "../renderer/renderer.h"

int main(void)
{
    EngineInit(MAX_PARTICLE);

    if (RendererInit() != 0)
        return 1;

    float dt = 0.1f;
    int running = 1;

    while (running)
    {
        running = RendererPollEvents();
        EngineStep(dt);
        RendererRender(EngineGetParticles(), EngineGetParticleCount());
    }

    EngineCleanup();
    RendererShutdown();
    return 0;
}
