#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdbool.h>
#include <raylib.h>
#include "../game/signal_type.h"

typedef struct
{
    int num_of_threads;
    int target_fps;
    float target_tps;
    bool pause;
    float zoom;
    Vector2 camera_position;
    Vector2 last_mouse_position;
    bool dark_theme;
    const Color* palette;
} settings_t;

void set_num_threads(settings_t* settings, int num_threads);
void set_theme(settings_t* settings, bool is_dark);
void set_zoom(settings_t* settings, float new_zoom);

inline Color get_signal_color(const settings_t* settings, const SignalType signal)
{
    return settings->palette[signal];
}

#endif
