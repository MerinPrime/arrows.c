#include "../../include/core/settings.h"
#include <omp.h>
#include "../../include/render/themes.h"

void set_num_threads(settings_t* settings, const int num_threads)
{
    settings->num_of_threads = num_threads;
    omp_set_num_threads(num_threads);
}

void set_theme(settings_t* settings, const bool is_dark)
{
    settings->dark_theme = is_dark;
    if (settings->dark_theme)
        settings->palette = dark_palette;
    else
        settings->palette = light_palette;
}

void set_zoom(settings_t* settings, const float new_zoom)
{
    const float screen_center_x = (float) GetScreenWidth() / 2.0f;
    const float screen_center_y = (float) GetScreenHeight() / 2.0f;
    const float dx = screen_center_x / settings->zoom - screen_center_x / new_zoom;
    const float dy = screen_center_y / settings->zoom - screen_center_y / new_zoom;
    settings->camera_position.x += dx;
    settings->camera_position.y += dy;
    settings->zoom = new_zoom;
}