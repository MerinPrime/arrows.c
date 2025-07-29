#ifndef ARROW_H
#define ARROW_H

#include <stdbool.h>
#include <stdint.h>
#include "arrow_type.h"
#include "signal_type.h"
#include "direction.h"
#include "handlers.h"

// TIP: On map editing just refind edges and detectors at neighbours ( 5x5 relative to edited arrow )

typedef struct arrow_t
{
    ArrowType type;
    Direction direction;
    bool flipped;

    uint8_t signal_count;
    uint8_t block_count;
    SignalType signal;
    SignalType detector_signal;

    SignalType last_signal;

    bool entry_point;
    bool changed;

    struct arrow_t* edges[MAX_EDGES];
    uint8_t edges_count;
    struct arrow_t* detectors[MAX_DETECTORS];
    uint8_t detectors_count;
} arrow_t;

#endif
