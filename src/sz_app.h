#ifndef _SZ_COMMON_H_
#define _SZ_COMMON_H_

#include <stdio.h>
#include <raylib.h>

#define DEFAULT_SCREEN_WIDTH 1280
#define DEFAULT_SCREEN_HEIGHT 720

struct sz_app;

struct sz_app_update_context {
    Vector2 mouse_world_pos;
    float zoom;
};

typedef int (*sz_app_update_fn)(void *arg, const struct sz_app_update_context *context);
typedef int (*sz_app_draw_fn)(void *arg, const Camera2D *camera);

struct sz_app *sz_app_init(sz_app_update_fn update_fn, void *update_arg, sz_app_draw_fn draw_fn, void *draw_arg);

void sz_app_loop(struct sz_app *app);

void sz_app_shutdown(struct sz_app *app);

#endif /* _SZ_COMMON_H_ */
