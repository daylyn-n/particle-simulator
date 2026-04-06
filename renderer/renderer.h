#ifndef RENDERER_H
#define RENDERER_H

#include "../engine/engine.h"

int RendererInit(void);
int RendererPollEvents(void);
void RendererRender(Particle *particles[], int count);
void RendererShutdown(void);

#endif
