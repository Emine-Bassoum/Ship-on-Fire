#include "front/user_interface.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
// Define the buttons
typedef struct {
    SDL_Rect rect;
    const char *text;
} Button;

Button buttons[3] = {
    {{0, 20, 200, 50}, "Go to loop.c"},
    {{0, 80, 200, 50}, "Pay respects to developer"},
    {{0, 140, 200, 50}, "Exit"},
};

int main(/* int argc, char *argv[] */) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    // Calculate the center of the window
    int centerX = WINDOW_WIDTH / 2;
    int centerY = WINDOW_HEIGHT / 2;

    SDL_Window *window = SDL_CreateWindow("Main Menu", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                                          WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    TTF_Font *font = TTF_OpenFont("../images/Arial.ttf", 40);
    if (!font) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        // handle error
    }
    SDL_Color color = {0, 0, 0, 255};
    SDL_Event event;
    int running = 1;

    while (running) {
    for (int i = 0; i < 3; i++) {
        buttons[i].rect.x = centerX - buttons[i].rect.w / 2;
        buttons[i].rect.y = centerY - buttons[i].rect.h / 2 + i * 60;
        }   
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);

                for (int i = 0; i < 3; i++) {
                    if (SDL_PointInRect(&(SDL_Point){x, y}, &buttons[i].rect)) {
                        printf("%s button clicked\n", buttons[i].text);
                        // Add your button click handling code here
                        switch (i) {
                        case 0: // "Go to loop.c" button
                            // Code to start the game or whatever "Go to loop.c"
                            // should do
                            system("./loop"); // This will launch the loop
                                              // executable
                            
                            break;
                        case 1: // "Pay respects to developer" button
                            // Code to show a message or whatever "Pay respects
                            // to developer" should do
                            printf("Paying respects to the developer...\nWho have suffered enough\n");
                            // Here you should call the function that shows the
                            // message
                            break;
                        case 2:          // "Exit" button
                            running = 0; // This will break the main loop and
                                         // exit the program
                            break;
                        }
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < 3; i++) {
            SDL_Surface *surface =
                TTF_RenderText_Solid(font, buttons[i].text, color);
            SDL_Texture *texture =
                SDL_CreateTextureFromSurface(renderer, surface);

            SDL_RenderCopy(renderer, texture, NULL, &buttons[i].rect);

            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}