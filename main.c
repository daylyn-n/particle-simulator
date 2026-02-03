#include <stdio.h>
#include <stdbool.h> 
#include <SDL2/SDL.h>
#include <math.h>
#include <time.h>

#define WIDTH 900
#define HEIGHT 600
#define MAX_PARTICLE 5
#define GRAVITY 9.8
#define FRICTION 0.1

enum RUNNING
{
    IS_RUNNING, NOT_RUNNING
};
int gRunning = IS_RUNNING ;
typedef struct
{
   float x, y, r, vx, vy, mass;

}Particle;
int randHelp(int lb, int ub)
{
    return (rand() % (ub - lb + 1)) + 1;

}
// x^2 + y^2 = r^2
// dx = +- sqrt(r*2 - y^2)
// get the x points of the circle to draw a line connectng the left point and right point
void drawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius)
{
    // Simple bounding check (optional)
    if (radius <= 0) return;

      
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
    if(x +  radius > WIDTH) // right wall
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
        particle->vy = -particle->vy/ 2;
    }
    
   
}
// two particles collide if the 
// distance between centers is <= radii
// sqrt(distX^2 + distY^2) <= radii 
// distX^2 + distY^2 <= radii^2
bool ParticlesCollide(Particle *p1, Particle *p2)
{
    float distX = p1->x - p2->x;
    float distY = p1->y - p2->y;
    float r     = p1->r + p2->r;
    return (distX*distX + distY*distY) <= (r * r);
}
void ResolveCollision(Particle *p1, Particle* p2)
{
     float distX = p1->x - p2->x;
     float distY = p1->y - p2->y;

     float distCenter = distX*distX + distY*distY;
    if (distCenter == 0.0f) {
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

     // COR, COEFFIENCE OF RESTITUION, actual bouncing time
     float e = 0.5f; // how elsatic our particles are
        
     // impulse
     float massOne = 1.0f / p1->mass;
     float massTwo = 1.0f / p2->mass;

     // Newtons third law :laugh: 
     // every action, there is an equal and opposite reaction
     float j = -(1 + e) * velAlongNormal;
     j /= (massOne + massTwo);

     // impulse along the normal direction!!!!
     float impulseX = j * nx;
     float impulseY = j * ny;

     p1->vx -= impulseX * massOne;
     p1->vy -= impulseY * massOne;
     p2->vx += impulseX * massTwo;
     p2->vy += impulseY * massTwo;
    
     // position corrections
     float overlap = (p1->r + p2->r) - radDist;
    if (overlap > 0) {
        float percent = 0.8f; // stabilization
        float correction = overlap * percent / (massOne + massTwo);

        p1->x -= correction * massOne * nx;
        p1->y -= correction * massOne * ny;
        p2->x += correction * massTwo * nx;
        p2->y += correction * massTwo * ny;
    }
}

void CollideAllParticle(Particle *particle)
{
    for(int i = 0; i < MAX_PARTICLE; i++)
    {
        for(int j = i + 1; j < MAX_PARTICLE; j++)
        {
            if(ParticlesCollide(&particle[i], &particle[j]))
            {
                ResolveCollision(&particle[i], &particle[j]);
            }
        }
    }
}
void MainLoop(SDL_Window *window, SDL_Renderer *renderer, Particle particle[], float deltaTime)
{
    const int FRAME_DELAY = 1000 / 60; 
    SDL_Event e;

    while(!gRunning)
    {
        Uint32 frameStart = SDL_GetTicks(); // gets the number of milisecdons since sdl initialuzed
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT)
            {
                printf("End");
                gRunning = NOT_RUNNING;
            }
        }

        // clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        // draw scene   
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for(int i = 0; i < MAX_PARTICLE; i++)
        {
            UpdateParticle(&particle[i], deltaTime); // collides with walls
        }

        CollideAllParticle(particle); // collides with other particles

        for(int i = 0; i < MAX_PARTICLE; i++)
        {
            DrawParticle(renderer, &particle[i]);
        }
        
        // updating the screen after any renders have been made
        SDL_RenderPresent(renderer);

        // cap frames
        Uint32 frameTime = SDL_GetTicks() - frameStart;

        if (frameTime < FRAME_DELAY)
        SDL_Delay(FRAME_DELAY - frameTime);
    }


}

void CleanUp(SDL_Window* window, SDL_Renderer *renderer)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}


int main()
{
    Particle particle[MAX_PARTICLE];
    srand(time(NULL));
    for(int i = 0; i < MAX_PARTICLE; i++)
    {
        particle[i].x = randHelp(100,300);
        particle[i].y = randHelp(100,300);
        particle[i].vx = randHelp(0,10);
        particle[i].vy = randHelp(0,10);
        particle[i].r = randHelp(50,70);
        particle[i].mass = 200;
    }
    float deltaTime = 0.1;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Particle Simulator", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    
    SDL_Renderer  *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); 
    MainLoop(window,renderer, particle, deltaTime);

    CleanUp(window, renderer);

    

    
}
