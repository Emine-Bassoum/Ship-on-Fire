#include "../back/player.h"
#include <string.h>
#include <assert.h>

// check if the player created is really defined
int main() {
    player_t *player = create_player("human_name", true);
    player_t *bot = create_player("bot_name", false);

    assert(player->is_human);
    assert(!bot->is_human);
    assert(!strcmp(player->name, "human_name"));

    free_player(player);
    free_player(bot);
    return 0;
}