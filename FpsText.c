#include "FpsText.h"

void FpsTextInit(FpsText *ft, SDL_Renderer *renderer)
{
    ft->font =  TTF_OpenFont("BitcountSingle-Regular.ttf", 24);
    ft->color = (SDL_Color){40, 255, 40};
    ft->textBox = (SDL_Rect){900, 700, 0, 0};
    ft->tex = NULL;
    ft->lastFpsVal = (Uint32)-1;
    
}

void DrawFpsText(FpsText *ft, SDL_Renderer *renderer, Uint32 fps)
{
    char msg[64];
    snprintf(msg, sizeof(msg), "FPS: %u", fps);
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(ft->font, msg, ft->color);

    ft->tex = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    ft->textBox.w = surfaceMessage->w;
    ft->textBox.h = surfaceMessage->h;
    SDL_RenderCopy(renderer, ft->tex, NULL, &ft->textBox);
}
