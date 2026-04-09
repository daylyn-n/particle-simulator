#ifndef RENDERER_H
#define RENDERER_H

#include "../engine/engine.h"

typedef struct {
    int   reset_requested;
    int   particle_count;
    float elasticity;
} RendererGuiState;

int               RendererInit(int initial_particle_count, float initial_elasticity);
int               RendererPollEvents(void);
void              RendererRender(Particle *particles[], int count);
RendererGuiState  RendererGetGuiState(void);
void              RendererShutdown(void);

#endif
