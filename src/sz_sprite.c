#include "sz_sprite.h"

#include <stdio.h>
#include <malloc.h>
#include <errno.h>
#include <math.h>

static inline uint16_t
get_frame_count(const char *path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Failed to open file %s\n", path);
        return 0;
    }

    char line[256];
    fread(line, sizeof(char), sizeof(line), fp);
    fclose(fp);

    return (uint16_t)strtoul(line, NULL, 10);
}

int sz_sprite_load(const struct sz_sprite_config *config, struct sz_sprite *sprite) {
    const struct sz_sprite_dir dir = sz_sprite_dir_create(config->path);

    const char *sheet_path = sz_sprite_dir_concat(&dir, "sheet.png");
    if (sheet_path == NULL) {
        fprintf(stderr, "Error: Failed to concatenate path for sprite sheet\n");
        return -ENOMEM;
    }

    const char *count_path = sz_sprite_dir_concat(&dir, "count.txt");
    if (count_path == NULL) {
        fprintf(stderr, "Error: Failed to concatenate path for sprite count\n");
        return -ENOMEM;
    }

    Image image = LoadImage(sheet_path);
    if (image.data == NULL) {
        fprintf(stderr, "Error: Failed to load image from %s\n", sheet_path);
        free(sprite);
        return -ENOENT;
    }

    sprite->texture = LoadTextureFromImage(image);
    if (sprite->texture.id == 0) {
        fprintf(stderr, "Error: Failed to load texture from %s\n", sheet_path);
        free(sprite);
        return -EINVAL;
    }

    sprite->frame_rec = (Rectangle){0, 0, config->width, config->height};
    sprite->current_frame = 0;
    sprite->elapsed = 0.0f;
    sprite->n_frames = get_frame_count(count_path);
    sprite->frame_time = config->total_time / (float)sprite->n_frames;

    free((char *)sheet_path);
    free((char *)count_path);
    free(dir.s);

    return 0;
}

void sz_sprite_unload(struct sz_sprite *sprite) {
    UnloadTexture(sprite->texture);
}

void sz_sprite_update(struct sz_sprite *sprite) {
    const float dt = GetFrameTime();

    sprite->elapsed += dt;
    if (sprite->elapsed >= sprite->frame_time) {
        if (++sprite->current_frame >= sprite->n_frames)
            sprite->current_frame = 0;

        sprite->elapsed = 0.0f;
        sprite->frame_rec.x = sprite->current_frame * sprite->frame_rec.width;
    }
}

Vector2 sz_sprite_size(const struct sz_sprite *sprite) {
    return (Vector2){
        .x = sprite->frame_rec.width,
        .y = sprite->frame_rec.height
    };
}

void sz_sprite_flip(struct sz_sprite *sprite, int direction) {
    sprite->direction = direction;
}

void sz_sprite_draw(struct sz_sprite *sprite, Vector2 position, float rotation) {
    Rectangle src = sprite->frame_rec;
    if (sprite->direction) {
        src.x += src.width;
        src.width = -src.width;
    }

    DrawTexturePro(
        sprite->texture,
        src,
        (Rectangle){ position.x, position.y, fabsf(src.width), src.height },
        (Vector2){ fabsf(src.width) / 2, src.height / 2 },
        rotation,
        WHITE
    );
}
