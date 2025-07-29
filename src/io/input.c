#include "../../include/io/input.h"
#include "../../include/io/events.h"
#include "../../include/core/main.h"
#include "../../include/game/arrow.h"
#include "../../include/game/map.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb_ds.h"

void handle_input(settings_t* settings, map_t* map) {
    if (IsKeyPressed(ARROW_NORTH)) {
        printf("key pressed\n");
    }
    if (IsKeyPressed(ARROW_SOUTH)) {
        printf("key pressed\n");
    }
    if (IsKeyPressed(ARROW_WEST)) {
        printf("key pressed\n");
    }
    if (IsKeyPressed(ARROW_EAST)) {
        printf("key pressed\n");
    }
    if (IsKeyPressed(ARROW_FLIP)) {
        printf("key pressed\n");
    }
    if (IsKeyPressed(ARROW_REMOVE)) {
        printf("key pressed\n");
    }
    if (IsKeyPressed(ARROW_MENU)) {
        printf("key pressed\n");
    }
    if (IsKeyPressed(SELECT)) {
        printf("key pressed\n");
    }
    if (IsKeyPressed(SELECTED_REMOVE)) {
        printf("key pressed\n");
    }
    if (IsKeyPressed(SELECTED_COPY)) {
        printf("key pressed\n");
    }
    if (IsKeyPressed(CLIPBOARD_PASTE)) {
        printf("key pressed\n");
    }
    if (IsKeyPressed(HOTBAR1)) {
        printf("key pressed\n");
    }
    if (IsKeyPressed(HOTBAR2)) {
        printf("key pressed\n");
    }
    if (IsKeyPressed(HOTBAR3)) {
        printf("key pressed\n");
    }
    if (IsKeyPressed(HOTBAR4)) {
        printf("key pressed\n");
    }
    if (IsKeyPressed(HOTBAR5)) {
        printf("key pressed\n");
    }
    if (IsKeyPressed(HOTBAR_DESELECT)) {
        printf("key pressed\n");
    }
    if (IsKeyPressed(HOTBAR_PREVIOUS)) {
        printf("key pressed\n");
    }
    if (IsKeyPressed(HOTBAR_NEXT)) {
        printf("key pressed\n");
    }
    if (IsKeyPressed(PIPETTE)) {
        printf("key pressed\n");
    }
    if (IsKeyDown(ZOOM_IN)) {
        set_zoom(settings, settings->zoom * ZOOM_FACTOR);
    }
    if (IsKeyDown(ZOOM_OUT)) {
        set_zoom(settings, settings->zoom / ZOOM_FACTOR);
    }
    const float wheel_move = GetMouseWheelMove();
    if (wheel_move != 0.0f)
    {
        const float new_zoom = __max(roundf(settings->zoom + wheel_move), 0.5);
        set_zoom(settings, new_zoom);
    }
    if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(ZOOM_RESET)) {
        settings->zoom = DEFAULT_ZOOM;
    }
    if (IsKeyPressed(SIGNALS_REMOVE)) {
        map_reset(map);
    }
    if (IsKeyPressed(TICK_STEP)) {
        map_update_single(map);
    }
    if (IsKeyPressed(PAUSE)) {
        settings->pause = !settings->pause;
    }
    if (IsKeyPressed(MAP_MENU)) {
        printf("key pressed\n");
    }
    if (IsKeyDown(MOVE_FORWARD)) {
        settings->camera_position.y -= CAMERA_SPEED;
    }
    if (IsKeyDown(MOVE_BACK)) {
        settings->camera_position.y += CAMERA_SPEED;
    }
    if (IsKeyDown(MOVE_LEFT)) {
        settings->camera_position.x -= CAMERA_SPEED;
    }
    if (IsKeyDown(MOVE_RIGHT)) {
        settings->camera_position.x += CAMERA_SPEED;
    }
    if (IsMouseButtonDown(CAMERA_MOVE)) {
        Vector2 mouse_position = GetMousePosition();
        Vector2 mouse_bias = {
            mouse_position.x - settings->last_mouse_position.x,
            mouse_position.y - settings->last_mouse_position.y
        };

        settings->camera_position.x -= mouse_bias.x / settings->zoom;
        settings->camera_position.y -= mouse_bias.y / settings->zoom;
    }
    if (IsKeyPressed(CENTER)) {
        settings->camera_position = (Vector2){
            .x = -(float) GetScreenWidth() / 2.0f / settings->zoom,
            .y = -(float) GetScreenHeight() / 2.0f / settings->zoom,
        };
    }

    if (IsMouseButtonReleased(ARROW_INTERACT)) {
        const int button_x = (int) roundf(settings->camera_position.x + settings->last_mouse_position.x / settings->zoom);
        const int button_y = (int) roundf(settings->camera_position.y + settings->last_mouse_position.y / settings->zoom);
        arrow_t* arrow = map_try_get_arrow(map, button_x, button_y);

        if (arrow != NULL)
        {
            if (arrow->type == AR_BUTTON || arrow->type == AR_DIRECTIONAL_BUTTON) {
                arrow->signal = S_ORANGE;
                arrpush(map->changed_nodes, arrow);
            }
        }
    }

    settings->last_mouse_position = GetMousePosition();
}
