#ifndef MAIN_H
#define MAIN_H

#define UNLOAD_TIMER_MAX 5
#define MAX_TPS 10000000
#define UI_BACKGROUND_ALPHA 0.9f
#define CAMERA_SPEED 2.0f
#define DEFAULT_ZOOM 16
#define ZOOM_FACTOR 1.01f

#ifndef DEBUG
#define DEBUG 0
#endif

typedef struct {
    enum QueueType {
        QT_POWER, QT_BLOCK
    } type;
    int32_t pos_x;
    int32_t pos_y;
} queue_t;

#endif
