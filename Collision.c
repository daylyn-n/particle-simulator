#include "Collision.h"
#include "Constants.h"
// x^2 + y^2 = r^2
// dx = +- sqrt(r*2 - y^2)
// get the x points of the circle to draw a line connectng the left point and right point
void drawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius)
{
    for (int y = -radius; y <= radius; y++) {
        int dx = (int)floor(sqrt((double)(radius * radius - y * y)));
        SDL_RenderDrawLine(renderer,
                           cx - dx, cy + y,
                           cx + dx, cy + y);
    }
}

void DrawParticle(SDL_Renderer *renderer, Particle *particle)
{
    drawFilledCircle(renderer, particle->x, particle->y, particle->r);
}
void UpdateParticle(Particle *particle, float deltaTime)
{
    particle->x += particle->vx * deltaTime;
    particle->y += particle->vy * deltaTime;

    float x = particle->x;
    float y = particle->y;
    float radius = particle->r;
    
    // simulate gravity 
    // sdl uses the top left corner as its origin
    // so we add gravity to make it go down
    particle->vy +=  GRAVITY * deltaTime;
    
    
    // apply friction
   if(particle->y == 0)
   {
        particle->vx -= FRICTION * particle->vx * deltaTime;
        if(fabs(particle->vx) < 1)
        {
            particle->vx = 0;
        }
   }

    // collision handling
    if(x - radius < 0) // left wall
    {
        particle->x = radius;
        particle->vx = -particle->vx;
    }
    if(x + radius > WIDTH) // right wall
    {
        particle->x = WIDTH - radius;
        particle->vx = -particle->vx;
    }
    if(y - radius < 0) // top bound
    {
        particle->y = radius;
        particle->vy = -particle->vy;
    }
    if(y + radius >  HEIGHT) // bottom bound
    {
        // bounce height based on mass
        
        particle->y = HEIGHT - radius;
        if(particle->vy > 0)
        {
            particle->vy = -particle->vy * gCOR;
        }
    }
}

// two particles collide if the 
// distance between centers is <= radii
// sqrt(distX^2 + distY^2) <= radii 
// distX^2 + distY^2 <= radii^2
bool ParticlesCollide(SDL_Renderer *renderer, Particle *p1, Particle *p2)
{
    float distX = p1->x - p2->x;
    float distY = p1->y - p2->y;
    float r     = p1->r + p2->r;

    return (distX*distX + distY*distY) <= (r * r);
}
void ResolveCollision(Particle *p1, Particle *p2)
{
    float distX = p1->x - p2->x;
    float distY = p1->y - p2->y;

    float distCenter = distX*distX + distY*distY;
    if (distCenter == 0.0f)
    {
        return;
    }
    float radDist = sqrtf(distCenter);
     
     // the distance between two centers normalized
     // in linear algebra, normalization allows us to
     // scale the magnitude (collision impulse) of our two particles
     // without changing the direction it goes in
     float nx = distX / radDist;
     float ny = distY / radDist;   

     // relative velocity
     float rvx = p1->vx - p2->vx;
     float rvy = p1->vy - p2->vy;

     float velAlongNormal = rvx * nx + rvy * ny;
     if(velAlongNormal > 0) // double bounce check
        return;

        
     // impulse
     float massOne = 1.0f / p1->mass;
     float massTwo = 1.0f / p2->mass;

     // Newtons third law :laugh: 
     // every action, there is an equal and opposite reaction
     float j = -(1 + gCOR) * velAlongNormal;
     j /= (massOne + massTwo);

     // impulse along the normal direction!!!!
     float impulseX = j * nx;
     float impulseY = j * ny;
    
   // deltaV = j / mass
     // our normal is the direction from
     // p2 -> p1
     // so p1 gets pushed forward along the Normal 
     // and p2 gets pushed away
     p1->vx += impulseX * massOne;
     p1->vy += impulseY * massOne;
     p2->vx -= impulseX * massTwo;
     p2->vy -= impulseY * massTwo;
    
    // position corrections
    float overlap = (p1->r + p2->r) - radDist;
    if (overlap > 0)
    {
        float percent = 0.8f; // stabilization
        float correction = overlap * percent / (massOne + massTwo);

        p1->x += correction * massOne * nx;
        p1->y += correction * massOne * ny;
        p2->x -= correction * massTwo * nx;
        p2->y -= correction * massTwo * ny;
    }
}

void DrawParticleColor(Particle *p1, Particle* p2)
{
    p1->collideColor = (SDL_Color){255, 80, 80, 255};
    p2->collideColor = (SDL_Color){255, 80, 80, 255};
    
}
void CollideAllParticle(SDL_Renderer* renderer, Particle *particle)
{
    for(int i = 0; i < MAX_PARTICLE; i++)
    {
        for(int j = i + 1; j < MAX_PARTICLE; j++)
        {
            if(ParticlesCollide(renderer, &particle[i], &particle[j]))
            {
                ResolveCollision(&particle[i], &particle[j]);
                DrawParticleColor(&particle[i], &particle[j]);
                SDL_SetRenderDrawColor(renderer,
                                    particle[i].collideColor.r,
                                    particle[i].collideColor.g,
                                    particle[i].collideColor.b,
                                    255);
            }
        }
    }
}

