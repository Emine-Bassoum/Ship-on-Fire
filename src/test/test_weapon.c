#include "../back/ship.h"
#include "../back/weapon.h"
#include <SDL2/SDL_rect.h>
#include <stdbool.h>
#include <stdio.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

// test the feature on the turrets
int main() {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_Window *window = SDL_CreateWindow(
        "Ship on FIRE!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    if (!window) {
        printf("Window error");
        exit(EXIT_FAILURE);
    }
    // rendering with Vsync on for Framerate to sync with screen frequency
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_SOFTWARE);

    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n",
               SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // test the creation of turrets
    SDL_Point point1 = {0, 0};
    SDL_Point point2 = {1, 0};
    SDL_Point point3 = {0, 1};
    SDL_Point point4 = {1, 1};
    ship *boat =
        create_new_ship(NULL, SHIP_TYPE_CORVETTE, point1, renderer, false);
    ship *boat2 =
        create_new_ship(NULL, SHIP_TYPE_CORVETTE, point1, renderer, false);
    turret *cannon1 =
        create_turret(TURRET_TYPE_AUTOCANNON, point1, boat, TOWARDS_BOW, true);
    turret *cannon2 = create_turret(TURRET_TYPE_AUTOCANNON, point2, boat,
                                    TOWARDS_STERN, true);
    turret *cannon3 = create_turret(TURRET_TYPE_AUTOCANNON, point3, boat2,
                                    TOWARDS_BOW, false);
    turret *cannon4 = create_turret(TURRET_TYPE_AUTOCANNON, point4, boat2,
                                    TOWARDS_STERN, false);
    shell *bullet1 = create_shell(cannon1);
    printf("etat du cannon 1 :%d\n", cannon1->state);
    printf("etat du cannon 2 :%d\n", cannon2->state);
    printf("etat du cannon 3 :%d\n", cannon3->state);
    printf("etat du cannon 4 :%d\n", cannon4->state);
    // Now we're gonna make some modification on the cannons
    turret_fire_damage(cannon1, 1); // test damage if not in fire state
    cannon2->health = 01;
    turret_lost_health(cannon2, bullet1); // test destroyed cannon
    cannon3->time_on_fire = 2;
    turret_fire_damage(cannon3, 1);       // test damge if in fire
    turret_lost_health(cannon4, bullet1); // test shoot against another cannon
    turret_lost_health(cannon4, bullet1);
    turret_lost_health(cannon4, bullet1);
    turret_lost_health(cannon4, bullet1);
    turret_lost_health(cannon4, bullet1);
    turret_lost_health(cannon4, bullet1);
    turret_lost_health(cannon4, bullet1);
    printf("etat du cannon 1 après actions:%d vie:%d\n", cannon1->state,
           cannon1->health);
    printf("etat du cannon 2 après actions:%d vie:%d\n", cannon2->state,
           cannon2->health);
    printf("etat du cannon 3 après actions:%d vie:%d\n", cannon3->state,
           cannon3->health);
    printf("etat du cannon 4 après actions:%d vie:%d\n", cannon4->state,
           cannon4->health);
    free_turret(cannon1);
    free_turret(cannon2);
    free_turret(cannon3);
    free_turret(cannon4);
    shell_destroy(bullet1);

    free_ship(boat);
    free_ship(boat2);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
