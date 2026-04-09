#ifndef GUI_H
#define GUI_H

#include <SDL2/SDL.h>

typedef struct {
    int   reset_requested;
    int   particle_count;
    float elasticity;
} GuiState;

#ifdef __cplusplus
extern "C" {
#endif

void     GuiInit(SDL_Window *window, SDL_Renderer *renderer, int initial_particle_count, float initial_elasticity);
void     GuiShutdown(void);
void     GuiHandleEvent(SDL_Event *event);
void     GuiNewFrame(void);
void     GuiRender(void);
GuiState GuiGetState(void);

#ifdef __cplusplus
}
#endif

#endif /* GUI_H */
