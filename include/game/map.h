#ifndef MAP_H
#define MAP_H

#include <stdint.h>
#include "point.h"
#include "chunk.h"
#include "../core/main.h"
#include "../io/buffer.h"

typedef struct {
    uint16_t version;
    struct ChunkHashTable { point_t key; chunk_t* value; }* chunks;
    arrow_t** entry_points;
    arrow_t** changed_nodes;
    arrow_t** temp_changed_nodes;
    bool restarted;
} map_t;

// TODO: May be use data oriented design?

chunk_t* map_get_chunk(map_t* map, point_t chunk_pos);
chunk_t* map_try_get_chunk(map_t* map, point_t chunk_pos);
arrow_t* map_get_arrow(map_t* map, int32_t x, int32_t y);
arrow_t* map_try_get_arrow(map_t* map, int32_t x, int32_t y);
void map_arrow_recompile(map_t* map, arrow_t* arrow, int x, int y);
void map_recompile(map_t* map);
void map_reset(map_t* map);
void map_clear(map_t* map);
void map_load(map_t* map, buffer_t* buffer);
void map_update(map_t* map);
void map_update_single(map_t* map);
void map_import(map_t* map, const char* input);

#endif
