#ifndef _H_SPRITE_H_
#define _H_SPRITE_H_

#include <raylib.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define SZ_SPRITE_MAX_ACTION_FRAMES 32

struct sz_sprite {
    Texture2D texture;          /* Texture of the sprite */
    Rectangle frame_rec;        /* Rectangle to select the current_frame */
    unsigned int current_frame; /* Current animation current_frame */
    unsigned int n_frames;      /* Number of frames in the sprite sheet */
    float frame_time;           /* Time per current_frame */
    float elapsed;              /* Accumulated time */
    int direction;              /* Horizontal direction of the sprite */
};

/* todo: move to slice_t */
struct sz_sprite_dir {
    size_t len;
    union {
        const char *cs;
        char *s;
    };
};

struct sz_sprite_config {
    const char *path;   /* Path to the sprite sheet */
    float width;        /* Width of each frame */
    float height;       /* Height of each frame */
    float total_time;   /* Total time for the animation */
};

int sz_sprite_load(const struct sz_sprite_config *config, struct sz_sprite *sprite);
void sz_sprite_unload(struct sz_sprite *sprite);
void sz_sprite_update(struct sz_sprite *sprite);

/* 0 - right, 1 - left */
void sz_sprite_flip(struct sz_sprite *sprite, int direction);

void sz_sprite_draw(struct sz_sprite *sprite, Vector2 position, float rotation);
Vector2 sz_sprite_size(const struct sz_sprite *sprite);

static inline struct sz_sprite_dir
sz_sprite_dir_create(const char *path) {
    struct sz_sprite_dir dir = {0};
    dir.len = strlen(path);
    dir.s = malloc(dir.len + 1);
    if (dir.s == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for sprite directory\n");
        return dir;
    }
    strncpy(dir.s, path, dir.len);
    dir.s[dir.len] = '\0';
    return dir;
}

static inline void
sz_sprite_dir_free(struct sz_sprite_dir *dir) {
    if (dir->s != NULL) {
        dir->len = 0;
        free(dir->s);
        dir->s = NULL;
    }
}

static inline const char *
sz_sprite_dir_concat(const struct sz_sprite_dir *dir, const char *path) {
    size_t path_len = strlen(path);
    char *full_path = malloc(dir->len + path_len + 2); // +2 for '/' and '\0'
    if (full_path == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for full path\n");
        return NULL;
    }
    snprintf(full_path, dir->len + path_len + 2, "%s/%s", dir->s, path);

    return full_path;
}

#endif /* _H_SPRITE_H_ */
