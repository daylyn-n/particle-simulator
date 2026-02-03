#include <stdio.h>
#include <stdbool.h> 
#include <SDL2/SDL.h>
#include <math.h>


#define WIDTH 900
#define HEIGHT 600
#define MAX_PARTICLE 1000
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
        for(int i = 0; i < 2; i++)
        {
            UpdateParticle(&particle[i], deltaTime);
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

   Particle particle[2] = 
   {
        {300, 300, 50, 7, 0},
        {200, 150, 20, 9, 0}
   };
    float deltaTime = 0.1;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Particle Simulator", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    
    SDL_Renderer  *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); 
    MainLoop(window,renderer, particle, deltaTime);

    CleanUp(window, renderer);

    

    
}
