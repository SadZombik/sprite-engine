#ifndef _SZ_ACTION_H_
#define _SZ_ACTION_H_

#include "utils.h"

enum sz_action_type {
    SZ_ACTION_IDLE = 0,
    
    /* Movement */
    SZ_ACTION_WALK,
    SZ_ACTION_RUN,

    /* 4 types of attack (adjust if needed) */
    SZ_ACTION_ATTACK_0,
    SZ_ACTION_ATTACK_1,
    SZ_ACTION_ATTACK_2,
    SZ_ACTION_ATTACK_3,

    /* Special actions */
    SZ_ACTION_JUMP,
    SZ_ACTION_DYING,

    SZ_ACTION_COUNT,
};

struct sz_action_type_move {
    Vector2 target;
};

struct sz_action_type_attack {
    Vector2 target;
    float damage;
};

struct sz_action_entry {
    TAILQ_ENTRY(sz_action_entry) entries;
    
    enum sz_action_type type;
    union {
        struct sz_action_type_move move;
        struct sz_action_type_attack attack;
        /* todo: add more */
    };
};

#endif /* _SZ_ACTION_H_ */
