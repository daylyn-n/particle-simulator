#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>
#include "constants.h"

typedef struct {
    unsigned char r, g, b, a;
} Color;

typedef struct {
    float *x, *y, *r, *vx, *vy, *mass;
    Color *color;
    Color *baseColor;
    Color *collideColor;
    float *colorTime;
} Particle;

void EngineInit(int particleCount);
void EngineStep(float dt);
Particle **EngineGetParticles(void);
int EngineGetParticleCount(void);
void EngineCleanup(void);
void EngineReset(int particleCount);
void EngineSetElasticity(float e);

#endif
