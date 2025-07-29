#ifndef CHUNK_H
#define CHUNK_H

#include "arrow.h"
#include "point.h"
#include "../core/main.h"

#define CHUNK_SIZE 16

typedef struct {
    arrow_t arrows[CHUNK_SIZE*CHUNK_SIZE];
} chunk_t;

inline point_t pos2chunk(int32_t x, int32_t y) {
    return (point_t) {
        .x = (x >= 0 ? x : x - CHUNK_SIZE + 1) / CHUNK_SIZE,
        .y = (y >= 0 ? y : y - CHUNK_SIZE + 1) / CHUNK_SIZE,
    };
}
arrow_t* chunk_get_arrow(chunk_t* chunk, int32_t x, int32_t y);

#endif
