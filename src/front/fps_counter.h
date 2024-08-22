#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H

#include <SDL2/SDL.h>

typedef struct FPSCounter {
    Uint32 frameCount;
    float frameRate;
    Uint32 lastTime;
} FPSCounter;

void FPSCounter_Init(FPSCounter* fpsCounter);
void FPSCounter_Tick(FPSCounter* fpsCounter);
float FPSCounter_GetFPS(FPSCounter* fpsCounter);

#endif // FPSCOUNTER_H