#include "../../include/game/chunk.h"

arrow_t* chunk_get_arrow(chunk_t* chunk, const int32_t x, const int32_t y)
{
    return &chunk->arrows[y*CHUNK_SIZE+x];
}