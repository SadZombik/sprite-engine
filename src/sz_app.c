#include "sz_app.h"

#include <stdio.h>
#include <malloc.h>

struct sz_app {
    Camera2D camera;
    Vector2 mouse_pos;

    float width;
    float height;

    float cam_speed;
    float edge_margin;

    /* user defined callbacks */
    sz_app_update_fn update_fn;
    sz_app_draw_fn draw_fn;

    /* user defined arguments */
    void *update_arg;
    void *draw_arg;
};

static inline void
update_state(struct sz_app *app) {
    const float dt = GetFrameTime();
    
    /* Keyboard controls */
    if (IsKeyDown(KEY_UP)) app->camera.target.y -= 1.0f;
    if (IsKeyDown(KEY_DOWN)) app->camera.target.y += 1.0f;
    if (IsKeyDown(KEY_LEFT)) app->camera.target.x -= 1.0f;
    if (IsKeyDown(KEY_RIGHT)) app->camera.target.x += 1.0f;

    /* Edge scrolling */
    app->mouse_pos = GetMousePosition();
    if (app->mouse_pos.y <= app->edge_margin) app->camera.target.y -= app->cam_speed * dt;              // Top edge
    if (app->mouse_pos.y >= app->height - app->edge_margin) app->camera.target.y += app->cam_speed * dt;  // Bottom edge
    if (app->mouse_pos.x <= app->edge_margin) app->camera.target.x -= app->cam_speed * dt;              // Left edge
    if (app->mouse_pos.x >= app->width - app->edge_margin) app->camera.target.x += app->cam_speed * dt;   // Right edge

    /* Mouse wheel for zoom */
    const float wheel_move = GetMouseWheelMove();
    if (wheel_move) {
        /* todo: magic numbers */
        app->camera.zoom += wheel_move * 0.1f;
        if (app->camera.zoom < 0.1f) app->camera.zoom = 0.1f;
        if (app->camera.zoom > 3.0f) app->camera.zoom = 3.0f;
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        const Vector2 mouse_delta = GetMouseDelta();
        app->camera.target.x -= mouse_delta.x / app->camera.zoom;
        app->camera.target.y -= mouse_delta.y / app->camera.zoom;
    }
}

struct sz_app *sz_app_init(sz_app_update_fn update_fn, void *update_arg, sz_app_draw_fn draw_fn, void *draw_arg) {
    InitWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, "Animated Sprites");
    SetTargetFPS(60);
    SetExitKey(KEY_ESCAPE);

    struct sz_app *app = malloc(sizeof(struct sz_app));
    if (app == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for app state\n");
        return NULL;
    }

    app->width = (float)GetScreenWidth();
    app->height = (float)GetScreenHeight();
    app->camera = (Camera2D){0};
    app->cam_speed = 300.0f;
    app->edge_margin = 20.0f;
    app->camera.target = (Vector2){0, 0};
    app->camera.offset = (Vector2){0, 0};
    app->camera.zoom = 1.0f;
    app->update_fn = update_fn;
    app->draw_fn = draw_fn;
    app->update_arg = update_arg;
    app->draw_arg = draw_arg;

    return app;
}

void sz_app_loop(struct sz_app *app) {
    while (!WindowShouldClose()) {
        update_state(app);

        const Vector2 mouse_world_pos = GetScreenToWorld2D(app->mouse_pos, app->camera);
        const struct sz_app_update_context context = {
            .mouse_world_pos = mouse_world_pos,
            .zoom = app->camera.zoom,
        };

        if (app->update_fn(app->update_arg, &context) != 0) {
            fprintf(stderr, "Error: Failed to update\n");
            break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (app->draw_fn(app->draw_arg, &app->camera) != 0) {
            fprintf(stderr, "Error: Failed to draw\n");
            break;
        }

        EndDrawing();
    }
}

void sz_app_shutdown(struct sz_app *app) {
    free(app);
    CloseWindow();
}
