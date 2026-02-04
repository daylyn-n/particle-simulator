#ifndef FPSTEXT_H
#define FPSTEXT_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


typedef struct
{
    TTF_Font *font;
    SDL_Rect textBox;
    SDL_Texture *tex;
    SDL_Color color;
    Uint32 lastFpsVal;
}FpsText;
 
void FpsTextInit(FpsText *ft, SDL_Renderer *renderer);


void DrawFpsText(FpsText *ft, SDL_Renderer *renderer, Uint32 fps);


#endif

