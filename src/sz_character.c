#include "sz_character.h"
#include "sz_sprite.h"
#include "sz_action.h"

#include <raymath.h>

#define MAX_ACTIONS 16

static const size_t ACTIONS_ARRAY_SIZE = sizeof(struct sz_action_entry) * MAX_ACTIONS;

TAILQ_HEAD(actions_queue, sz_action_entry);

struct sz_character {
    char name[SZ_CHARACTER_NAME_MAX];

    struct sz_sprite sprites[SZ_ACTION_COUNT];
    size_t current_sprite;

    Vector2 position;
    float rotation;
    float velocity;

    size_t actions_count;
    struct sz_action_entry actions[MAX_ACTIONS];
    struct actions_queue actions_queue;
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

    for (size_t i = 0; i < config->n_sprite_configs; i++) {
        const size_t index = config->sprite_configs[i].type;
        rc = sz_sprite_load(&config->sprite_configs[i], &character->sprites[index]);
        if (rc != 0)
            return NULL;
    }

    character->velocity = 100.0f;
    TAILQ_INIT(&character->actions_queue);

    return character;
}

void sz_character_unload(struct sz_character *character) {
    for (uint16_t i = 0; i < SZ_ACTION_COUNT; i++)
        sz_sprite_unload(&character->sprites[i]);
}

static int command_move(struct sz_character *character, struct sz_action_type_move *move, float dt) {
    Vector2 direction = Vector2Subtract(move->target, character->position);

    const float distance = Vector2Length(direction);
    const float speed = character->velocity * dt;

    direction = Vector2Scale(Vector2Normalize(direction), speed);
    character->position = Vector2Add(character->position, direction);

    return Vector2Equals(character->position, move->target) || distance <= speed;
}

static int command_attack(struct sz_character *character, struct sz_action_type_attack *attack) {
    
    
    return 0;
}

void sz_character_update(struct sz_character *character, float zoom, Vector2 mouse_world_pos) {
    /* Move */
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        memset(character->actions, 0, ACTIONS_ARRAY_SIZE);
        character->actions_count = 0;
        character->current_sprite = SZ_ACTION_RUN;

        const Vector2 mouse_pos = GetMousePosition();

        struct sz_action_entry *entry = &character->actions[character->actions_count++];
        entry->type = SZ_ACTION_RUN;
        entry->move.target = mouse_world_pos;
        DrawCircleV(mouse_pos, 10, RED);

        /* 0 - right, 1 - left */
        const int direction = entry->move.target.x < character->position.x;

        sz_sprite_flip(&character->sprites[SZ_ACTION_RUN], direction);
        sz_sprite_flip(&character->sprites[SZ_ACTION_IDLE], direction);

        TAILQ_INSERT_TAIL(&character->actions_queue, entry, entries);
    }

    /* Attack */
    if (IsKeyPressed(KEY_A)) {
        memset(character->actions, 0, ACTIONS_ARRAY_SIZE);
        character->actions_count = 0;
        character->current_sprite = SZ_ACTION_ATTACK_0;

        struct sz_action_entry *entry = &character->actions[character->actions_count++];
        entry->type = SZ_ACTION_ATTACK_0;
        /* todo: setup context */

        TAILQ_INSERT_TAIL(&character->actions_queue, entry, entries);
    }

    /* Process commands */
    if (character->actions_count) {
        const float dt = GetFrameTime();
        struct sz_action_entry *entry = TAILQ_FIRST(&character->actions_queue);
        if (entry != NULL) {
            switch (entry->type) {
                case SZ_ACTION_RUN:
                    if (command_move(character, &entry->move, dt)) {
                        TAILQ_REMOVE(&character->actions_queue, entry, entries);
                        character->actions_count--;
                    }
                    break;
                case SZ_ACTION_ATTACK_0:
                    if (command_attack(character, &entry->attack)) {
                        TAILQ_REMOVE(&character->actions_queue, entry, entries);
                        character->actions_count--;
                    }
                    break;
                default:
                    fprintf(stderr, "Warning: Unhandled action type %d\n", entry->type);
                    TAILQ_REMOVE(&character->actions_queue, entry, entries);
                    break;
            }
        }
    } else {
        /* Idle */
        character->current_sprite = SZ_ACTION_IDLE;
    }

    sz_sprite_update(&character->sprites[character->current_sprite]);
}

void sz_character_draw(struct sz_character *character) {
    struct sz_sprite *sprite = &character->sprites[character->current_sprite];
    sz_sprite_draw(sprite, character->position, character->rotation);
}
