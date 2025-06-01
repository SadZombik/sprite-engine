#include "sz_character.h"
#include "sz_sprite.h"
#include "utils.h"

#include <raymath.h>

#define MAX_COMMANDS 16

enum command_type {
    COMMAND_TYPE_MOVE = 0,
    COMMAND_TYPE_ATTACK,
    COMMAND_TYPE_JUMP,
    COMMAND_TYPE_NONE
};

struct command_entry {
    TAILQ_ENTRY(command_entry) entries;

    enum command_type type;
    union {
        struct command_type_move {
            Vector2 target;
        } move;

        struct command_type_attack {
            Vector2 target; /* Target position for the attack */
            float damage;   /* Damage dealt by the attack */
        } attack;
    };
};

static const size_t COMMANDS_ARRAY_SIZE = sizeof(struct command_entry) * MAX_COMMANDS;

TAILQ_HEAD(command_queue, command_entry);

struct sz_character {
    char name[SZ_CHARACTER_NAME_MAX];

    size_t n_sprites;
    struct sz_sprite *sprites;
    size_t current_sprite;

    Vector2 position;
    float rotation;
    float velocity;

    size_t commands_count;
    struct command_entry commands[MAX_COMMANDS];
    struct command_queue commands_queue;
};

void sz_character_set_pos(struct sz_character *character, Vector2 position) {
    character->position = position;
}

Vector2 sz_character_get_pos(struct sz_character *character) {
    return character->position;
}

struct sz_character *sz_character_init(const struct sz_character_config *config) {
    int rc;

    struct sz_character *character = calloc(1, sizeof(struct sz_character));
    if (character == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for character\n");
        return NULL;
    }

    strncpy(character->name, config->name, SZ_CHARACTER_NAME_MAX);

    character->n_sprites = config->n_sprite_configs;
    character->sprites = calloc(character->n_sprites, sizeof(struct sz_sprite));
    if (character->sprites == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for sprites\n");
        free(character);
        return NULL;
    }

    for (size_t i = 0; i < character->n_sprites; i++) {
        rc = sz_sprite_load(&config->sprite_configs[i], &character->sprites[i]);
        if (rc != 0)
            return NULL;
    }

    character->velocity = 100.0f;
    TAILQ_INIT(&character->commands_queue);

    return character;
}

void sz_character_unload(struct sz_character *character) {
    for (uint16_t i = 0; i < character->n_sprites; i++)
        sz_sprite_unload(&character->sprites[i]);

    free(character->sprites);
}

static int command_move(struct sz_character *character, struct command_type_move *move, float dt) {
    Vector2 direction = Vector2Subtract(move->target, character->position);

    const float distance = Vector2Length(direction);
    const float speed = character->velocity * dt;

    direction = Vector2Scale(Vector2Normalize(direction), speed);
    character->position = Vector2Add(character->position, direction);

    return Vector2Equals(character->position, move->target) || distance <= speed;
}

static int command_attack(struct sz_character *character, struct command_type_attack *attack) {
    
    
    return 0;
}

void sz_character_update(struct sz_character *character, float zoom, Vector2 mouse_world_pos) {
    // if (IsKeyReleased(KEY_ONE)) 
    // if (IsKeyDown(KEY_TWO)) character->current_sprite = 1;
    // if (IsKeyDown(KEY_THREE)) character->current_sprite = 2;
    // if (IsKeyDown(KEY_FOUR)) character->current_sprite = 3;

    /* Move */
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        memset(character->commands, 0, COMMANDS_ARRAY_SIZE);
        character->commands_count = 0;
        character->current_sprite = 1;

        const Vector2 mouse_pos = GetMousePosition();

        struct command_entry *entry = &character->commands[character->commands_count++];
        entry->type = COMMAND_TYPE_MOVE;
        entry->move.target = mouse_world_pos;
        DrawCircleV(mouse_pos, 10, RED);

        const int direction = entry->move.target.x < character->position.x; // 0 - right, 1 - left

        sz_sprite_flip(&character->sprites[0], direction);
        sz_sprite_flip(&character->sprites[1], direction);

        TAILQ_INSERT_TAIL(&character->commands_queue, entry, entries);
    }

    /* Attack */
    if (IsKeyPressed(KEY_A)) {
        memset(character->commands, 0, COMMANDS_ARRAY_SIZE);
        character->commands_count = 0;
        character->current_sprite = 2;

        struct command_entry *entry = &character->commands[character->commands_count++];
        entry->type = COMMAND_TYPE_ATTACK;

        TAILQ_INSERT_TAIL(&character->commands_queue, entry, entries);
    }

    /* Process commands */
    if (character->commands_count) {
        const float dt = GetFrameTime();
        struct command_entry *entry = TAILQ_FIRST(&character->commands_queue);
        if (entry != NULL) {
            switch (entry->type) {
                case COMMAND_TYPE_MOVE:
                    if (command_move(character, &entry->move, dt)) {
                        TAILQ_REMOVE(&character->commands_queue, entry, entries);
                        character->commands_count--;
                    }
                    break;
                case COMMAND_TYPE_ATTACK:
                    if (command_attack(character, &entry->attack)) {
                        TAILQ_REMOVE(&character->commands_queue, entry, entries);
                        character->commands_count--;
                    }
                    break;
                case COMMAND_TYPE_JUMP:
                    // Handle jump command
                    break;
                case COMMAND_TYPE_NONE:
                    // No command
                    break;
            }
        }
    } else {
        /* Idle */
        character->current_sprite = 0;
    }

    sz_sprite_update(&character->sprites[character->current_sprite]);
}

void sz_character_draw(struct sz_character *character) {
    struct sz_sprite *sprite = &character->sprites[character->current_sprite];
    sz_sprite_draw(sprite, character->position, character->rotation);
}
