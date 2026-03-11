#include <stdio.h>
#include <stdlib.h>
#include "Collision.h"
#include "Constants.h"
#include "FpsText.h"
#include <time.h>
#include <valgrind/callgrind.h>


int gRunning = IS_RUNNING ;
int randHelp(int lb, int ub)
{
    return (rand() % (ub - lb + 1)) + 1;
}
SDL_Texture *CreateCircleTexture(SDL_Renderer* renderer, int radius)
{

    int size = radius * 2;
    
    SDL_Texture *tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, size, size);

    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(renderer, tex);

    SDL_SetRenderDrawColor(renderer, 0,0,0,0);

    SDL_RenderClear(renderer);
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    int cx = radius, cy = radius;
    for (int y = -radius; y <= radius; y++) {
        int dx = (int)floor(sqrt((double)(radius * radius - y * y)));
        SDL_RenderDrawLine(renderer,
                cx - dx, cy + y,
                cx + dx, cy + y);
    }
    SDL_SetRenderTarget(renderer, NULL);
    return tex;

}

void MainLoop(SDL_Window *window, SDL_Renderer *renderer, Particle *particle[], float deltaTime, FpsText ft)
{
    const int FRAME_DELAY = 1000 / 60; 
    SDL_Event e;
    
    Uint32 lastTime = SDL_GetTicks();
    Uint32 frames = 0;
    Uint32 fps = 0;
    Uint32 fpsTimer = SDL_GetTicks();
    FpsTextInit(&ft, renderer);
    SDL_Texture *circleTex = CreateCircleTexture(renderer, *particle[0]->r);
    
    while(!gRunning)
    {
        Uint32 frameStart = SDL_GetTicks();
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
        DrawFpsText(&ft, renderer, fps);
        for(int i = 0; i < MAX_PARTICLE; i++)
        {
            UpdateParticle(particle[i], deltaTime);
        }

        CollideAllParticle(renderer, particle);
        for(int i = 0; i < MAX_PARTICLE; i++)
        {
            SDL_Rect dest = {
                (int)*particle[i]->x - *particle[i]->r,
                (int)*particle[i]->y - *particle[i]->r,
                *particle[i]->r * 2, 
                *particle[i]->r * 2
            }; 
            SDL_RenderCopy(renderer, circleTex, NULL, &dest);
        }

        // PROFILER END
        CALLGRIND_STOP_INSTRUMENTATION;
        SDL_RenderPresent(renderer);

        // FPS calculation
        frames++;
        Uint32 now = SDL_GetTicks();
        if (now - fpsTimer >= 1000) {
            fps = frames;
            frames = 0;
            fpsTimer = now;
        }

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
    TTF_Quit();
    SDL_Quit();
}


int main()
{
    Particle *particle[MAX_PARTICLE];
    srand(time(NULL));
    // profiler START
    CALLGRIND_START_INSTRUMENTATION;
    for(int i = 0; i < MAX_PARTICLE; i++)
    {

        // Allocate memory for the Particle struct
        particle[i] = malloc(sizeof(Particle));
        
        // Allocate memory for each pointer member
        particle[i]->x           = malloc(sizeof(float));
        particle[i]->y           = malloc(sizeof(float));
        particle[i]->vx          = malloc(sizeof(float));
        particle[i]->vy          = malloc(sizeof(float));
        particle[i]->r           = malloc(sizeof(float));
        particle[i]->mass        = malloc(sizeof(float));
        particle[i]->color       = malloc(sizeof(SDL_Color));
        particle[i]->baseColor   = malloc(sizeof(SDL_Color));
        particle[i]->collideColor = malloc(sizeof(SDL_Color));
        particle[i]->colorTime   = malloc(sizeof(float));
        
        // Now assign values
        *particle[i]->x           = randHelp(10,890);
        *particle[i]->y           = randHelp(10,590);
        *particle[i]->vx          = randHelp(-20,20);
        *particle[i]->vy          = randHelp(-20,20);
        *particle[i]->r           = 10;
        *particle[i]->mass        = 80;
        *particle[i]->baseColor   = (SDL_Color){255, 255, 255, 255}; // white
        *particle[i]->colorTime   = 2.0f;
    }
    float deltaTime = 0.1;

    FpsText ft;
    TTF_Font* font  = TTF_OpenFont("BitcountSingle-Regular.ttf", 24);
    SDL_Init(SDL_INIT_VIDEO);
    
    if (TTF_Init() == -1)
    {
    printf("Failed to initialize SDL_ttf: %s\n", TTF_GetError());
         return 1;
    }
    
    SDL_Window* window = SDL_CreateWindow("Particle Simulator", SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer  *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); 
    //SDL_SetRenderDrawColor(renderer, randHelp(0,255), randHelp(0,255), randHelp(0,255), 255);
    MainLoop(window,renderer, particle, deltaTime, ft);
    CleanUp(window, renderer);    
}
