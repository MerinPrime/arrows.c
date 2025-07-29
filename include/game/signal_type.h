#ifndef SIGNAL_TYPE_H
#define SIGNAL_TYPE_H

typedef enum {
    S_NONE,
    S_RED,
    S_BLUE,
    S_YELLOW,
    S_ORANGE,
    S_DEBUG,

    S_BLOCK,
    S_DELAY_AFTER_RED,
} SignalType;

#endif
