#ifndef COLLISION_H
#define COLLISION_H

#include <SDL2/SDL.h>
#include <math.h>
#include <stdbool.h>
typedef struct
{
    float x, y, r, vx, vy, mass;
    SDL_Color color;
    SDL_Color baseColor;
    SDL_Color collideColor;

    float colorTime;

}Particle;
void drawFilledCircle(SDL_Renderer *renderer, int cx, int cy, int radius);
void DrawParticle(SDL_Renderer *renderer, Particle *particle);
void UpdateParticle(Particle* particle, float deltaTime);
bool ParticlesCollide(SDL_Renderer *renderer, Particle *p1, Particle *p2);
void ResolveCollision(Particle *p1, Particle *p2);
void DrawParticleColor(Particle *p1, Particle *p2);
void CollideAllParticle(SDL_Renderer *renderer, Particle *particle);
#endif

