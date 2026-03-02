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
    drawFilledCircle(renderer, *particle->x, *particle->y, *particle->r);
}
void UpdateParticle(Particle *particle, float deltaTime)
{
    *particle->x += *particle->vx * deltaTime;
    *particle->y += *particle->vy * deltaTime;

    float x = *particle->x;
    float y = *particle->y;
    float radius = *particle->r;
    
    // simulate gravity 
    // sdl uses the top left corner as its origin
    // so we add gravity to make it go down
    *particle->vy +=  GRAVITY * deltaTime;
    
    
    // apply friction
   if(*particle->y == 0)
   {
        *particle->vx -= FRICTION * *particle->vx * deltaTime;
        if(fabs(*particle->vx) < 1)
        {
            *particle->vx = 0;
        }
   }

    // collision handling
    if(x - radius < 0) // left wall
    {
        *particle->x = radius;
        *particle->vx *= -1;
    }
    if(x + radius > WIDTH) // right wall
    {
        *particle->x = WIDTH - radius;
        *particle->vx *= -1;
    }
    if(y - radius < 0) // top bound
    {
        *particle->y = radius;
        *particle->vy *= -1;
    }
    if(y + radius >  HEIGHT) // bottom bound
    {
        // bounce height based on mass
        
        *particle->y = HEIGHT - radius;
        if(particle->vy > 0)
        {
            *particle->vy *= -1 * gCOR;
        }
    }
}

// two particles collide if the 
// distance between centers is <= radii
// sqrt(distX^2 + distY^2) <= radii 
// distX^2 + distY^2 <= radii^2
bool ParticlesCollide(SDL_Renderer *renderer, Particle *p1, Particle *p2)
{
    float distX = *p1->x - *p2->x;
    float distY = *p1->y - *p2->y;
    float r     = *p1->r + *p2->r;

    return (distX*distX + distY*distY) <= (r * r);
}
void ResolveCollision(Particle *p1, Particle *p2)
{
    float distX = *p1->x - *p2->x;
    float distY = *p1->y - *p2->y;

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
     float rvx = *p1->vx - *p2->vx;
     float rvy = *p1->vy - *p2->vy;

     float velAlongNormal = rvx * nx + rvy * ny;
     if(velAlongNormal > 0) // double bounce check
        return;

        
     // impulse
     float massOne = 1.0f / *p1->mass;
     float massTwo = 1.0f / *p2->mass;

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
     *p1->vx += impulseX * massOne;
     *p1->vy += impulseY * massOne;
     *p2->vx -= impulseX * massTwo;
     *p2->vy -= impulseY * massTwo;
    
    // position corrections
    float overlap = (*p1->r + *p2->r) - radDist;
    if (overlap > 0)
    {
        float percent = 0.8f; // stabilization
        float correction = overlap * percent / (massOne + massTwo);

        *p1->x += correction * massOne * nx;
        *p1->y += correction * massOne * ny;
        *p2->x -= correction * massTwo * nx;
        *p2->y -= correction * massTwo * ny;
    }
}

void DrawParticleColor(Particle *p1, Particle* p2)
{
    *p1->collideColor = (SDL_Color){255, 80, 80, 255};
    *p2->collideColor = (SDL_Color){255, 80, 80, 255};
    
}

// Helper to get primary quadrant for a particle (the quadrant its center is in)
static inline int getPrimaryQuadrant(Particle *p, float halfWidth, float halfHeight)
{
    int qx = (*p->x >= halfWidth) ? 1 : 0;
    int qy = (*p->y >= halfHeight) ? 1 : 0;
    return qy * 2 + qx;
}

void CollideAllParticle(SDL_Renderer* renderer, Particle *particle[])
{
    // Use dynamic allocation for large particle counts
    int **quadrantParticles = malloc(NUM_QUADRANTS * sizeof(int*));
    int *quadrantCounts = calloc(NUM_QUADRANTS, sizeof(int));
    
    for (int q = 0; q < NUM_QUADRANTS; q++)
    {
        quadrantParticles[q] = malloc(MAX_PARTICLE * sizeof(int));
    }
    
    float halfWidth = (float)WIDTH / 2.0f;
    float halfHeight = (float)HEIGHT / 2.0f;
    
    // Phase 1: Distribute particles into quadrants
    // Particles near boundaries are added to multiple quadrants
    for (int i = 0; i < MAX_PARTICLE; i++)
    {
        float x = *particle[i]->x;
        float y = *particle[i]->y;
        float r = *particle[i]->r;
        
        bool inLeft = (x - r < halfWidth);
        bool inRight = (x + r >= halfWidth);
        bool inTop = (y - r < halfHeight);
        bool inBottom = (y + r >= halfHeight);
        
        if (inLeft && inTop)
            quadrantParticles[0][quadrantCounts[0]++] = i;
        if (inRight && inTop)
            quadrantParticles[1][quadrantCounts[1]++] = i;
        if (inLeft && inBottom)
            quadrantParticles[2][quadrantCounts[2]++] = i;
        if (inRight && inBottom)
            quadrantParticles[3][quadrantCounts[3]++] = i;
    }
    
    // Phase 2: Check collisions within each quadrant
    // To avoid duplicate checks, only process a pair in the lowest quadrant
    // where BOTH particles' centers reside
    for (int q = 0; q < NUM_QUADRANTS; q++)
    {
        for (int i = 0; i < quadrantCounts[q]; i++)
        {
            for (int j = i + 1; j < quadrantCounts[q]; j++)
            {
                int idx1 = quadrantParticles[q][i];
                int idx2 = quadrantParticles[q][j];
                
                // Get primary quadrant for each particle (where their center is)
                int q1 = getPrimaryQuadrant(particle[idx1], halfWidth, halfHeight);
                int q2 = getPrimaryQuadrant(particle[idx2], halfWidth, halfHeight);
                
                // Only process this pair if current quadrant is the minimum of their primary quadrants
                // This ensures each pair is checked exactly once
                int minQ = (q1 < q2) ? q1 : q2;
                if (q != minQ)
                    continue;
                
                if (ParticlesCollide(renderer, particle[idx1], particle[idx2]))
                { 
                    ResolveCollision(particle[idx1], particle[idx2]);
                    DrawParticleColor(particle[idx1], particle[idx2]);
                    SDL_SetRenderDrawColor(renderer,
                                        particle[idx1]->collideColor->r,
                                        particle[idx1]->collideColor->g,
                                        particle[idx1]->collideColor->b,
                                        255);
                }
            }
        }
    }
    
    // Cleanup
    for (int q = 0; q < NUM_QUADRANTS; q++)
    {
        free(quadrantParticles[q]);
    }
    free(quadrantParticles);
    free(quadrantCounts);
}
