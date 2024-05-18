#ifndef KEYBOARD
#define KEYBOARD

// 实现功能：
/**
 * 1.另外开启一个线程，监测键盘状态
 * 2.公开键盘状态
*/

#include "SDL2/SDL.h"
#include "Timer.cpp"

const Uint8* KeyboardState;
SDL_Thread *pgetKBState;

void startListenKB();
int getKBState( void* );
bool ifPress(int);

void startListenKB() {
    pgetKBState = SDL_CreateThread(getKBState, "getkbstate", nullptr);
}
int getKBState( void *data ) {
    Timer gap = Timer(10);
    while(1){
        if (gap.check()){
            KeyboardState=SDL_GetKeyboardState(NULL);
        }
    }
    return 0;
}

bool ifPress(int scancode) {
    return KeyboardState[scancode];
}

#endif