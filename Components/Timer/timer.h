#ifndef TIMER_H
#define TIMER_H

#include <SDL2/SDL.h>

// SDL timer callback that posts an SDL_USEREVENT
Uint32 sdl_timer_callback(Uint32 interval, void* param);

#endif // TIMER_H
