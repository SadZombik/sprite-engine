#ifndef _SZ_CHARACTER_H_
#define _SZ_CHARACTER_H_

#include "sz_sprite.h"

#define SZ_CHARACTER_NAME_MAX 32
#define SZ_CHARACTER_ACTIONS_MAX 32

struct sz_character;

struct sz_character_config {
    char name[SZ_CHARACTER_NAME_MAX];
    size_t n_sprite_configs;
    struct sz_sprite_config *sprite_configs;
};

struct sz_character *sz_character_init(const struct sz_character_config *config);

void sz_character_set_pos(struct sz_character *character, Vector2 position);
Vector2 sz_character_get_pos(struct sz_character *character);

void sz_character_update(struct sz_character *character, float zoom, Vector2 mouse_world_pos);
void sz_character_draw(struct sz_character *character);
void sz_character_unload(struct sz_character *character);

#endif /* _SZ_CHARACTER_H_ */
