#include "renderer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <math.h>

static SDL_Window *window = NULL;
static SDL_Renderer *sdl_renderer = NULL;
static SDL_Texture *circleTex = NULL;

/* FPS text state */
static TTF_Font *fpsFont = NULL;
static SDL_Color fpsColor = {40, 255, 40, 255};
static SDL_Rect fpsTextBox = {10, 10, 0, 0};
static Uint32 fpsFrames = 0;
static Uint32 fps = 0;
static Uint32 fpsTimer = 0;

static const int FRAME_DELAY = 1000 / 60;

static SDL_Texture *CreateCircleTexture(int radius)
{
    int size = radius * 2;

    SDL_Texture *tex = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA8888,
                                         SDL_TEXTUREACCESS_TARGET, size, size);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(sdl_renderer, tex);
    SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 0);
    SDL_RenderClear(sdl_renderer);

    SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255);
    int cx = radius, cy = radius;
    for (int y = -radius; y <= radius; y++)
    {
        int dx = (int)floor(sqrt((double)(radius * radius - y * y)));
        SDL_RenderDrawLine(sdl_renderer, cx - dx, cy + y, cx + dx, cy + y);
    }
    SDL_SetRenderTarget(sdl_renderer, NULL);
    return tex;
}

static void DrawFpsText(void)
{
    char msg[64];
    snprintf(msg, sizeof(msg), "FPS: %u", fps);
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(fpsFont, msg, fpsColor);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(sdl_renderer, surfaceMessage);
    fpsTextBox.w = surfaceMessage->w;
    fpsTextBox.h = surfaceMessage->h;
    SDL_RenderCopy(sdl_renderer, tex, NULL, &fpsTextBox);
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(tex);
}

int RendererInit(void)
{
    SDL_Init(SDL_INIT_VIDEO);

    if (TTF_Init() == -1)
    {
        printf("Failed to initialize SDL_ttf: %s\n", TTF_GetError());
        return -1;
    }

    window = SDL_CreateWindow("Particle Simulator", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    sdl_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    fpsFont = TTF_OpenFont("BitcountSingle-Regular.ttf", 24);
    fpsTimer = SDL_GetTicks();

    circleTex = CreateCircleTexture(PARTICLE_RADIUS);

    return 0;
}

int RendererPollEvents(void)
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT)
        {
            printf("End");
            return 0;
        }
    }
    return 1;
}

void RendererRender(Particle *particles[], int count)
{
    Uint32 frameStart = SDL_GetTicks();

    // clear screen
    SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
    SDL_RenderClear(sdl_renderer);

    // draw FPS
    SDL_SetRenderDrawColor(sdl_renderer, 255, 255, 255, 255);
    DrawFpsText();

    // draw particles
    for (int i = 0; i < count; i++)
    {
        SDL_Rect dest = {
            (int)*particles[i]->x - *particles[i]->r,
            (int)*particles[i]->y - *particles[i]->r,
            *particles[i]->r * 2,
            *particles[i]->r * 2
        };
        SDL_RenderCopy(sdl_renderer, circleTex, NULL, &dest);
    }

    SDL_RenderPresent(sdl_renderer);

    // FPS calculation
    fpsFrames++;
    Uint32 now = SDL_GetTicks();
    if (now - fpsTimer >= 1000)
    {
        fps = fpsFrames;
        fpsFrames = 0;
        fpsTimer = now;
    }

    // cap frames
    Uint32 frameTime = SDL_GetTicks() - frameStart;
    if (frameTime < FRAME_DELAY)
        SDL_Delay(FRAME_DELAY - frameTime);
}

void RendererShutdown(void)
{
    if (circleTex) SDL_DestroyTexture(circleTex);
    if (fpsFont) TTF_CloseFont(fpsFont);
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}
