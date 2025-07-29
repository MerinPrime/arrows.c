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
    struct arrow_t* edges[MAX_EDGES];
    struct arrow_t* detectors[MAX_DETECTORS];

    uint8_t type;
    uint8_t direction;

    uint8_t signal_count;
    uint8_t block_count;

    uint8_t signal;
    uint8_t detector_signal;
    uint8_t last_signal;

    bool flipped;
    bool entry_point;
    bool changed;

    uint8_t edges_count;
    uint8_t detectors_count;
} arrow_t;

#endif
