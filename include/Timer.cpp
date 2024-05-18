#ifndef TIMER_CPP
#define TIMER_CPP

#include <SDL2/SDL.h>

struct Timer {
	unsigned int last,tick;
	Timer (int _tick);
	bool check(); //check and update
	bool ocheck(); // only check
	void update();
};
Timer :: Timer (int _tick){
	last = 0;
	tick = _tick;
}
bool Timer :: check() { //check and update
	unsigned int now = SDL_GetTicks();
	if (now > last + tick) {
		last = now;
		return 1;
	}
	return 0;
}
bool Timer :: ocheck() { // only check
	unsigned int now = SDL_GetTicks();
	if (now > last + tick) {
		return 1;
	}
	return 0;
}
void Timer :: update() {
	unsigned int now = SDL_GetTicks();
	last = now;
}

#endif