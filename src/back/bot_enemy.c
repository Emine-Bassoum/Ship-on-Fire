#include "bot_enemy.h"
#include "geometry.h"
#include "player.h"
#include "ship.h"
#include <math.h>
#include <stdio.h>

void bot_action(player_t *ai_player, int *cooldown_counter,
                player_t *human_player, shell_list *list) {

    ship *human_ship = human_player->controlled_ship;
    ship *ai_ship = ai_player->controlled_ship;

    int distance = -(human_ship->coordinates.x - ai_ship->coordinates.x);

    // get closer to human
    if (distance < 2 * human_ship->width - 200) {
        ai_ship->chadburn_setting = CHADBURN_SPEED_FULL_REVERSE;

    } else if (distance < -2 * human_ship->width - 100) {
        ai_ship->chadburn_setting = CHADBURN_SPEED_HALF_REVERSE;

    } else if (distance < -human_ship->width - 200) {
        ai_ship->chadburn_setting = CHADBURN_SPEED_NEUTRAL;

    } else if (distance < human_ship->width + 200) {
        ai_ship->chadburn_setting = CHADBURN_SPEED_NEUTRAL;

    } else if (distance < 2 * human_ship->width + 200) {
        ai_ship->chadburn_setting = CHADBURN_SPEED_THIRD_AHEAD;

    } else {
        ai_ship->chadburn_setting = CHADBURN_SPEED_FULL_AHEAD;
    }

    // float angle = calculate_angle_to_player(ai_ship, human_ship);
    ai_ship->turrets[0]->cannon_angle =
        calculate_angle_to_player(ai_ship, human_ship);

    if (*cooldown_counter >= AI_COOLDOWN) {

        shell *bullet = create_shell(ai_ship->turrets[0]);
        shell_list_add_element(list, bullet);
        *cooldown_counter = 0;

    } else {
        (*cooldown_counter)++;
    }
}

double calculate_angle_to_player(ship *ai_ship, ship *human_ship) {
    double best_angle = ai_ship->turrets[0]->min_cannon_angle;
    double smallest_error = INFINITY;

    for (double angle = ai_ship->turrets[0]->min_cannon_angle;
         angle <= ai_ship->turrets[0]->max_cannon_angle; angle += 1.0) {
        double distance = simulate_shell_flight(ai_ship, angle, 12);

        double error = fmin(
            fabs(human_ship->coordinates.x - distance),
            fabs(human_ship->coordinates.x + human_ship->width - distance));

        if (error < smallest_error) {
            smallest_error = error;
            best_angle = angle;
        }
    }
    return best_angle;
}

double simulate_shell_flight(ship *ai_ship, double angle, int initial_speed) {




    // Convert angle to radians
    double angle_rad = deg_to_rad(angle);

    // Create a shell and set its initial position and speed
    shell s;
    s.coordinates.x = ai_ship->coordinates.x;
    s.coordinates.y = 0; // Assuming the shell is launched from the ground
    s.speed.x = initial_speed * cos(angle_rad);
    s.speed.y = initial_speed * sin(angle_rad);
    s.accumulated_speed_change = 0;

    
    // Simulate the flight of the shell until it hits the ground
    const float GRAVITY = 0.05f; // Define gravity constant

    while (s.coordinates.y >= 0) {

        // Accumulate speed change due to gravity
        s.accumulated_speed_change -= GRAVITY;

        if (s.accumulated_speed_change <= -1.0f) {

            s.speed.y += s.accumulated_speed_change; // Increase vertical speed
            s.accumulated_speed_change = 0; // Reset accumulated speed change
        }

        // Update position based on speed
        s.coordinates.x += s.speed.x;
        s.coordinates.y += s.speed.y;
    }

    // Return the final distance of the shell
    return s.coordinates.x;
}
