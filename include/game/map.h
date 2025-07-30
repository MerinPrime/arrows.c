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
//       If so i think to add reoptimization action that reorders data for a more linear graph
// TODO: Also try splitting `arrow_t** changed_nodes` into
//       `arrow_t** changed_red_source`, `arrow_t** changed_red_arrow`, etc.
//       to remove the switch in update_arrow.
// TODO: Implement cycle and timer optimizations from layers-dlc
// TODO: May be try make a JIT for arrow structures?
//       X -> NOT -> NOT -> Y to X -> 1 -> 2 -> Y
// TODO: Path compilation? Idk how, but i need to try this idea in layers-dlc
//       So i think this can be maked using delayed signal update
//       X->1->2->3->4->5->Y compile path to single arrow X->PATH->Y
//       And path can keep only bit for each arrow in the path and just shift result
//       For this example PATH data can be 01101 and every tick shifted by 1 like:
//       Tick 0: PATH = 01101 | Y = 0
//       Tick 1: PATH = 00110 | Y = 1
//       Tick 2: PATH = 00011 | Y = 0
//       Tick 3: PATH = 00001 | Y = 1
//       Tick 4: PATH = 00000 | Y = 1
//       Also if PATH does not update signal of Y we can skip storing of Y in changed_nodes
//       PATH must be contain only arrows like RED_ARROW, BLUE_ARROW, RED_SPLITTER_UP_RIGHT etc.
// TODO: Like path compilation i think to make arrow union
//       It will optimize structures like pixels or compile branches to single path
//       This refers to JIT
// TODO: Also i think about complex graph structure, like back links for arrow
//       Idk how good this idea btw in my mind this looks like Babel AST
// TODO: I want to make multithreading but currenly it decreases tps from 80k to 1.5k ( bruh )
// TODO: Remove detectors and replace by edges
// TODO: Change static edges array to dynamic array using stb_ds

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
