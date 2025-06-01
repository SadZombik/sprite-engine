#include "../sz_sprite.h"

#include <stdio.h>
#include <string.h>

struct sz_sprite_action {
    unsigned int n_frames;
    char frames[SZ_SPRITE_MAX_ACTION_FRAMES][FILENAME_MAX];
};

static inline void unload_images(Image *images, unsigned int count) {
    for (unsigned int i = 0; i < count; i++)
        UnloadImage(images[i]);
}

int sz_sprite_generate_sprite_sheet(const struct sz_sprite_action *action, const char *sprite_path, const char *count_path) {
    /* Load first image */
    Image first = LoadImage(action->frames[0]);
    if (first.data == NULL) {
        fprintf(stderr, "Error: Failed to load image from %s\n", action->frames[0]);
        return -EINVAL;
    }

    ImageResize(&first, 100, 100);
    const float width = (float)first.width;
    const float height = (float)first.height;

    /* Generate sprite sheet */
    Image sheet = GenImageColor((int)width * action->n_frames, (int)height, BLANK);
    if (sheet.data == NULL) {
        fprintf(stderr, "Error: Failed to generate image from %s\n", action->frames[0]);
        return -ENOMEM;
    }

    Image frames[SZ_SPRITE_MAX_ACTION_FRAMES]; /* Array to hold frames */
    for (unsigned int j = 0; j < action->n_frames; j++) {
        frames[j] = LoadImage(action->frames[j]);
        if (frames[j].data == NULL) {
            fprintf(stderr, "Error: Failed to load image from %s\n", action->frames[j]);
            unload_images(frames, action->n_frames);
            UnloadImage(sheet);
            return -EINVAL;
        }

        ImageResize(&frames[j], 100, 100);

        ImageDraw(&sheet, frames[j], (Rectangle){0, 0, width, height}, (Rectangle){j * width, 0, width, height}, WHITE);
        if (sheet.data == NULL) {
            fprintf(stderr, "Error: Failed to draw image from %s\n", action->frames[j]);
            unload_images(frames, action->n_frames);
            UnloadImage(sheet);
            return -ENOMEM;
        }
    }

    ExportImage(sheet, sprite_path);

    // sprite->texture = LoadTextureFromImage(sheet);
    // sprite->frameRec = (Rectangle){0, 0, 100, 100};
    // sprite->frame = 0;
    // sprite->frameTime = 0.1f;
    // sprite->elapsedTime = 0.0f;
    // sprite->position = (Vector2){200, 200};

    unload_images(frames, action->n_frames);
    UnloadImage(first);
    UnloadImage(sheet);

    FILE *count_fp = fopen(count_path, "w");
    if (count_fp == NULL) {
        fprintf(stderr, "Error: Failed to open file %s for writing\n", count_path);
        return -EIO;
    }

    char buf[64];
    snprintf(buf, sizeof(buf), "%u\n", action->n_frames);
    fwrite(buf, sizeof(char), strlen(buf), count_fp);
    fclose(count_fp);

    return 0;
}

int main(int argc, char *argv[]) {    
    if (argc < 4) {
        fprintf(stderr, "Usage: generate-sprite-sheet.exe <path1> <path2> ... -o <out.png> <out.txt>\n");
        return -1;
    } else if (argc - 2 > SZ_SPRITE_MAX_ACTION_FRAMES) {
        fprintf(stderr, "Error: Too many sprite sheets specified. Max is %d\n", SZ_SPRITE_MAX_ACTION_FRAMES);
        return -1;
    }

    struct sz_sprite_action action = {
        .n_frames = argc - 4,
        .frames = {0},
    };

    printf("Number of frames: %u\n", action.n_frames);
    for (int i = 0; i < argc; i++) {
        printf("arg %u: %s\n", i, argv[i]);
    }

    char sprite_path[FILENAME_MAX] = {0};
    char count_path[FILENAME_MAX] = {0};

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 2 >= argc) {
                fprintf(stderr, "Error: Missing output file names after -o\n");
                return -1;
            }
            strncpy(sprite_path, argv[i + 1], FILENAME_MAX);
            strncpy(count_path, argv[i + 2], FILENAME_MAX);

            printf("Output sprite sheet path: %s\n", sprite_path);
            printf("Output count path: %s\n", count_path);

            break;
        } else {
            strncpy(action.frames[i - 1], argv[i], FILENAME_MAX);
            printf("Input sprite sheet path: %s\n", argv[i]);
        }
    }

    return sz_sprite_generate_sprite_sheet(&action, sprite_path, count_path);
}
