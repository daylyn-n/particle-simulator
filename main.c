#include <stdio.h>
#include <stdbool.h> 
#include <SDL2/SDL.h>
#include <math.h>


#define WIDTH 900
#define HEIGHT 600
#define MAX_PARTICLE 1000
enum RUNNING
{
    IS_RUNNING, NOT_RUNNING
};
int gRunning = IS_RUNNING ;
typedef struct
{
   float x,y,r,vx,vy;

}Particle;

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
void UpdateParticle(Particle *particle)
{
    particle->x += particle->vx;
    particle->y += particle->vy;

    float x = particle->x;
    float y = particle->y;
    float radius = particle->r;
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
    if(y - radius < 0) // bottom bound
    {
        particle->y = radius;
        particle->vy = -particle->vy;
    }
    if(y + radius >  HEIGHT) // top bound
    {
        particle->y = HEIGHT - radius;
        particle->vy = -particle->vy;
    }
}
void Initialize()
{
    Particle particle = {300, 300, 50,-7, 7};

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Particle Simulator", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    
    
    const int FRAME_DELAY = 1000 / 60; 
    SDL_Event e;

    SDL_Renderer  *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); 
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
        UpdateParticle(&particle);
        DrawParticle(renderer, &particle);
        
        // updating the screen after any renders have been made
        SDL_RenderPresent(renderer);

        // cap frames
        Uint32 frameTime = SDL_GetTicks() - frameStart;

        if (frameTime < FRAME_DELAY)
        SDL_Delay(FRAME_DELAY - frameTime);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
}






int main()
{

    Initialize();

   // MainLoop();

    

    
}
