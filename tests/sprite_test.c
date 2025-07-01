#include <sz_sprite.h>
#include <sz_app.h>

#include <stdio.h>
#include <stdlib.h>

#define N_SPRITES 4U

#define SZ_SPRITES_FOREACH(fn, sprites, count, ...) \
    do { \
        for (size_t _i = 0; _i < (count); ++_i) { \
            fn(&(sprites)[_i], ##__VA_ARGS__); \
        } \
    } while (0)

static const char *dirs[N_SPRITES] = {
    "./res/dark-oracle/Actions/Dying",
    "./res/dark-oracle/Actions/Idle",
    "./res/dark-oracle/Actions/Running",
    "./res/dark-oracle/Actions/Walking",
};

static int init_config(struct sz_sprite_config *config) {
    const float width = 100.f, height = 100.0f;

    for (size_t i = 0; i < N_SPRITES; i++) {
        config[i].path = dirs[i];
        config[i].width = width,
        config[i].height = height,
        config[i].total_time = 1.0f;
    }

    return 0;
}

static int load_sprites(struct sz_sprite_config *config, struct sz_sprite *sprites) {
    int rc;

    for (size_t i = 0; i < N_SPRITES; i++) {
        rc = sz_sprite_load(&config[i], &sprites[i]);
        if (rc != 0)            
            return rc;
    }

    return 0;
}

static int update_sprites(void *arg, const struct sz_app_update_context *context) {
    struct sz_sprite *sprites = (struct sz_sprite *)arg;

    for (size_t i = 0; i < N_SPRITES; i++)
        sz_sprite_update(&sprites[i]);

    return 0;
}

static int draw_sprites(void *arg, const Camera2D *camera) {
    struct sz_sprite *sprites = (struct sz_sprite *)arg;

    BeginMode2D(*camera);
    for (size_t i = 0; i < N_SPRITES; i++)
        sz_sprite_draw(&sprites[i], (Vector2){i * 100.0f, 0}, 0.0f);

    EndMode2D();
    return 0;
}

int main() {
    struct sz_sprite_config configs[N_SPRITES];
    if (init_config(configs) != 0) {
        fprintf(stderr, "Failed to initialize sprite configurations\n");
        return -1;
    }

    struct sz_sprite sprites[N_SPRITES] = {0};

    void *app_state = sz_app_init(update_sprites, sprites, draw_sprites, sprites);
    if (app_state == NULL) {
        fprintf(stderr, "Failed to initialize app state\n");
        return -1;
    }

    if (load_sprites(configs, sprites) != 0) {
        fprintf(stderr, "Failed to load sprites\n");
        return -1;
    }

    sz_app_loop(app_state);

    SZ_SPRITES_FOREACH(sz_sprite_unload, sprites, N_SPRITES);
    sz_app_shutdown(app_state);

    return 0;
}
