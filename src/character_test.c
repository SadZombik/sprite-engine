#include "sz_character.h"
#include "sz_app.h"

#include <stdio.h>

#define N_CHARACTERS 4
static struct sz_character *characters[N_CHARACTERS] = {0};

#define SPRITE_WIDTH 100.0f
#define SPRITE_HEIGHT 100.0f

static struct sz_sprite_config sprite_configs[] = {
    {
        .path = "./res/dark-oracle/Actions/Idle",
        .width = SPRITE_WIDTH,
        .height = SPRITE_HEIGHT,
        .total_time = 1.0f,
        .type = SZ_ACTION_IDLE,
    },
    {
        .path = "./res/dark-oracle/Actions/Running",
        .width = SPRITE_WIDTH,
        .height = SPRITE_HEIGHT,
        .total_time = 1.0f,
        .type = SZ_ACTION_RUN,
    },
    {
        .path = "./res/dark-oracle/Actions/Slashing",
        .width = SPRITE_WIDTH,
        .height = SPRITE_HEIGHT,
        .total_time = 0.5f,
        .type = SZ_ACTION_ATTACK_0,
    },
    {
        .path = "./res/dark-oracle/Actions/Dying",
        .width = SPRITE_WIDTH,
        .height = SPRITE_HEIGHT,
        .total_time = 1.0f,
        .type = SZ_ACTION_DYING,
    }
};

static const struct sz_character_config dark_oracle_config = {
    .name = "Dark Oracle",
    .n_sprite_configs = sizeof(sprite_configs) / sizeof(sprite_configs[0]),
    .sprite_configs = sprite_configs,
};

static inline int update(void *arg, const struct sz_app_update_context *context) {
    struct sz_character **characters = (struct sz_character **)arg;
    for (size_t i = 0; i < N_CHARACTERS; i++)
        sz_character_update(characters[i], context->zoom, context->mouse_world_pos);
    return 0;
}

static inline int draw(void *arg, const Camera2D *camera) {
    struct sz_character **characters = (struct sz_character **)arg;

    BeginMode2D(*camera);
    for (size_t i = 0; i < N_CHARACTERS; i++)
        sz_character_draw(characters[i]);

    EndMode2D();
    return 0;
}

int main() {
    struct sz_app *app_state = sz_app_init(update, characters, draw, characters);
    if (app_state == NULL) {
        fprintf(stderr, "Failed to initialize app state\n");
        return -1;
    }

    for (size_t i = 0; i < N_CHARACTERS; i++) {
        characters[i] = sz_character_init(&dark_oracle_config);
        if (characters[i] == NULL) {
            fprintf(stderr, "Failed to initialize character %zu\n", i);
            return -1;
        }
        sz_character_set_pos(characters[i], (Vector2){SPRITE_WIDTH * i, SPRITE_WIDTH});
    }

    sz_app_loop(app_state);

    for (size_t i = 0; i < N_CHARACTERS; i++) {
        if (characters[i] != NULL)
            sz_character_unload(characters[i]);
    }
    sz_app_shutdown(app_state);

    return 0;
}
