#include "fps_counter.h"

void FPSCounter_Init(FPSCounter *fpsCounter) {
    fpsCounter->frameCount = 0;
    fpsCounter->frameRate = 0;
    fpsCounter->lastTime = SDL_GetTicks();
}

void FPSCounter_Tick(FPSCounter *fpsCounter) {
    fpsCounter->frameCount++;
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime > fpsCounter->lastTime + 1000) {
        fpsCounter->frameRate = fpsCounter->frameCount;
        fpsCounter->frameCount = 0;
        fpsCounter->lastTime = currentTime;
    }
}

float FPSCounter_GetFPS(FPSCounter *fpsCounter) {
    return fpsCounter->frameRate;
}