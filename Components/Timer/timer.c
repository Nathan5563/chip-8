#include <SDL2/SDL.h>
#include "timer.h"

Uint32 sdl_timer_callback(Uint32 interval, void* param) {
	SDL_Event event;
	event.type = SDL_USEREVENT;
	SDL_PushEvent(&event);
	return interval;
}
