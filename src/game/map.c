#include "../../include/game/map.h"

#include <assert.h>
#include <omp.h>
#include <stb_ds.h>
#include <stdatomic.h>
#include <stdio.h>

#include "stuff.h"
#include "../../lib/include/stb_ds.h"
#include "../../include/io/buffer.h"

chunk_t* map_get_chunk(map_t* map, const point_t chunk_pos) {
    const struct ChunkHashTable* kv = hmgetp_null(map->chunks, chunk_pos);
    if (kv == NULL) {
        chunk_t* new_chunk = calloc(1, sizeof(chunk_t));
        assert(new_chunk != NULL && "Cannot allocate new chunk, download more ram pls.");
        hmput(map->chunks, chunk_pos, new_chunk);
        return new_chunk;
    }
    return kv->value;
}

chunk_t* map_try_get_chunk(map_t* map, const point_t chunk_pos) {
    const struct ChunkHashTable* kv = hmgetp_null(map->chunks, chunk_pos);
    if (kv == NULL) return NULL;
    return kv->value;
}

arrow_t* map_get_arrow(map_t* map, int32_t x, int32_t y) {
    chunk_t* chunk = map_get_chunk(map, pos2chunk(x, y));
    if (x < 0) x *= -1;
    if (y < 0) y *= -1;
    x %= CHUNK_SIZE;
    y %= CHUNK_SIZE;
    return &chunk->arrows[y*CHUNK_SIZE+x];
}

arrow_t* map_try_get_arrow(map_t* map, int32_t x, int32_t y) {
    chunk_t* chunk = map_try_get_chunk(map, pos2chunk(x, y));
    if (chunk == NULL) return NULL;
    if (x < 0) x *= -1;
    if (y < 0) y *= -1;
    x %= CHUNK_SIZE;
    y %= CHUNK_SIZE;
    return &chunk->arrows[y*CHUNK_SIZE+x];
}

void map_clear(map_t* map)
{
    if (map->chunks == NULL) return;

    for (size_t i = 0; i < hmlen(map->chunks); ++i) {
        chunk_t* chunk = map->chunks[i].value;
        // for (int j = 0; j < CHUNK_SIZE * CHUNK_SIZE; ++j) {
            // arrow_t* arrow = &chunk->arrows[j];
            // free(arrow->edges);
            // arrow->edges = NULL;
            // free(arrow->detectors);
            // arrow->detectors = NULL;
        // }
        free(chunk);
    }
    hmfree(map->chunks);
    map->chunks = NULL;
    arrfree(map->entry_points);
    map->entry_points = NULL;
    arrfree(map->changed_nodes);
    map->changed_nodes = NULL;
    arrfree(map->temp_changed_nodes);
    map->temp_changed_nodes = NULL;
    map->version = 0;
    map->restarted = true;
}

static point_t get_pos_relative(map_t* map, int x, int y,
    const int forward, int diagonal, const Direction direction, const bool flipped)
{
    if (flipped) diagonal = -diagonal;
    if (direction == D_NORTH) {
        y += forward;
        x += diagonal;
    } else if (direction == D_EAST) {
        x -= forward;
        y += diagonal;
    } else if (direction == D_SOUTH) {
        y -= forward;
        x -= diagonal;
    } else if (direction == D_WEST) {
        x += forward;
        y -= diagonal;
    }

    return (point_t) { x, y };
}

static arrow_t* get_arrow_relative(map_t* map, int x, int y,
    const int forward, int diagonal, const Direction direction, const bool flipped)
{
    const point_t pos = get_pos_relative(map, x, y, forward, diagonal, direction, flipped);
    return map_try_get_arrow(map, pos.x, pos.y);
}

static void _store_edge(map_t* map, arrow_t* edges[], uint8_t* count, const int x, const int y,
    const int forward, int diagonal, const Direction direction, const bool flipped)
{
    arrow_t* arrow = get_arrow_relative(map, x, y, forward, diagonal, direction, flipped);
    if (arrow == NULL) return;
    if (is_not_allowed_to_change(arrow->type)) return;
    edges[(*count)++] = arrow;
}

static void _store_detector(map_t* map, arrow_t* edges[], uint8_t* count, const int x, const int y,
    const int forward, int diagonal, const Direction direction, const bool flipped)
{
    const point_t pos = get_pos_relative(map, x, y, forward, diagonal, direction, flipped);
    arrow_t* arrow = map_try_get_arrow(map, pos.x, pos.y);
    if (arrow == NULL) return;

    const point_t back_pos = get_pos_relative(map, pos.x, pos.y, 1, 0, arrow->direction, arrow->flipped);

    if (back_pos.x != x || back_pos.y != y) return;
    edges[(*count)++] = arrow;
}

void map_arrow_recompile(map_t* map, arrow_t* arrow, const int x, const int y) {
    // TODO: Recalculate signals on editing

    // if (arrow->edges != NULL) free(arrow->edges);
    // if (arrow->detectors != NULL) free(arrow->detectors);
    arrow->edges_count = 0;
    arrow->detectors_count = 0;

    #define store_edge(forward, diagonal)\
        _store_edge(map, arrow->edges, &arrow->edges_count, x, y, forward, diagonal, arrow->direction, arrow->flipped)
    #define store_detector(forward, diagonal)\
        _store_detector(map, arrow->detectors, &arrow->detectors_count, x, y, forward, diagonal, arrow->direction, arrow->flipped)

    switch (arrow->type) {
    case AR_EMPTY:
        break;
    case AR_ARROW:
        store_edge(-1, 0);
        break;
    case AR_SOURCE:
        store_edge(-1, 0);
        store_edge(0, 1);
        store_edge(1, 0);
        store_edge(0, -1);
        break;
    case AR_BLOCKER:
        store_edge(-1, 0);
        break;
    case AR_DELAY:
        store_edge(-1, 0);
        break;
    case AR_DETECTOR:
        store_edge(-1, 0);
        break;
    case AR_SPLITTER_UP_DOWN:
        store_edge(-1, 0);
        store_edge(1, 0);
        break;
    case AR_SPLITTER_UP_RIGHT:
        store_edge(-1, 0);
        store_edge(0, 1);
        break;
    case AR_SPLITTER_UP_RIGHT_LEFT:
        store_edge(0, -1);
        store_edge(-1, 0);
        store_edge(0, 1);
        break;
    case AR_IMPULSE:
        store_edge(-1, 0);
        store_edge(0, 1);
        store_edge(1, 0);
        store_edge(0, -1);
        break;
    case AR_BLUE_ARROW:
        store_edge(-2, 0);
        break;
    case AR_DIAGONAL_ARROW:
        store_edge(-1, 1);
        break;
    case AR_BLUE_SPLITTER_UP_UP:
        store_edge(-1, 0);
        store_edge(-2, 0);
        break;
    case AR_BLUE_SPLITTER_RIGHT_UP:
        store_edge(-2, 0);
        store_edge(0, 1);
        break;
    case AR_BLUE_SPLITTER_UP_DIAGONAL:
        store_edge(-1, 0);
        store_edge(-1, 1);
        break;
    case AR_LOGIC_NOT:
        store_edge(-1, 0);
        break;
    case AR_LOGIC_AND:
        store_edge(-1, 0);
        break;
    case AR_LOGIC_XOR:
        store_edge(-1, 0);
        break;
    case AR_LATCH:
        store_edge(-1, 0);
        break;
    case AR_FLIP_FLOP:
        store_edge(-1, 0);
        break;
    case AR_RANDOM:
        store_edge(-1, 0);
        break;
    case AR_BUTTON:
        store_edge(-1, 0);
        store_edge(0, 1);
        store_edge(1, 0);
        store_edge(0, -1);
        break;
    case AR_LEVEL_SOURCE:
        break;
    case AR_LEVEL_TARGET:
        break;
    case AR_DIRECTIONAL_BUTTON:
        store_edge(-1, 0);
        break;
    }

    store_detector(-1, 0);
    store_detector(0, 1);
    store_detector(1, 0);
    store_detector(0, -1);

    #undef store_edge
    #undef store_detector

    const bool entry_point = is_entry_point(arrow->type);
    if (!entry_point && arrow->entry_point) {
        const ptrdiff_t index = arrfind(map->entry_points, arrow);
        arrdelswap(map->entry_points, index);
    } else if (entry_point && !arrow->entry_point) {
        arrpush(map->entry_points, arrow);
    }
}

void map_recompile(map_t* map) {
    struct ChunkHashTable* chunks = map->chunks;
    if (chunks == NULL) return;

    for (size_t i = 0; i < hmlen(chunks); ++i) {
        chunk_t* chunk = chunks[i].value;
        for (int j = 0; j < CHUNK_SIZE * CHUNK_SIZE; ++j) {
            const int x = j % CHUNK_SIZE + chunks[i].key.x * CHUNK_SIZE;
            const int y = j / CHUNK_SIZE + chunks[i].key.y * CHUNK_SIZE;
            arrow_t* arrow = &chunk->arrows[j];
            arrow->signal_count = 0;
            arrow->block_count = 0;
            arrow->signal = S_NONE;
            arrow->detector_signal = S_NONE;
            map_arrow_recompile(map, arrow, x, y);
        }
    }
    const size_t len = arrlen(map->entry_points);
    if (arrcap(map->changed_nodes) < len)
        arrsetcap(map->changed_nodes, len);
    arrsetlen(map->changed_nodes, len);
    memcpy(map->changed_nodes, map->entry_points, len * sizeof(arrow_t*));
    map->restarted = true;
}

void map_reset(map_t* map) {
    if (map->chunks == NULL) return;

    for (size_t i = 0; i < hmlen(map->chunks); ++i) {
        chunk_t* chunk = map->chunks[i].value;
        for (int j = 0; j < CHUNK_SIZE * CHUNK_SIZE; ++j) {
            arrow_t* arrow = &chunk->arrows[j];
            arrow->signal_count = 0;
            arrow->block_count = 0;
            arrow->signal = S_NONE;
            arrow->last_signal = S_NONE;
            arrow->detector_signal = S_NONE;
        }
    }
    const size_t len = arrlen(map->entry_points);
    if (arrcap(map->changed_nodes) < len)
        arrsetcap(map->changed_nodes, len);
    arrsetlen(map->changed_nodes, len);
    memcpy(map->changed_nodes, map->entry_points, len * sizeof(arrow_t*));
    map->restarted = true;
}

void map_load(map_t* map, buffer_t* buffer) {
    assert(buffer != NULL && "Buffer ptr is NULL, download more ram pls.");
    map_clear(map);

    map->version = buf_pop16(buffer);
    const uint16_t chunk_count = buf_pop16(buffer);
    for (uint16_t _chunk = 0; _chunk < chunk_count; _chunk++) {
        const int16_t chunk_x = (int16_t) buf_pop16(buffer);
        const int16_t chunk_y = (int16_t) buf_pop16(buffer);
        chunk_t* chunk = map_get_chunk(map, (point_t){ chunk_x, chunk_y });
        const uint8_t types_count = buf_pop8(buffer) + 1;
        for(uint8_t _type = 0; _type < types_count; _type++) {
            const uint8_t arrow_type = buf_pop8(buffer);
            const uint8_t arrow_count = buf_pop8(buffer) + 1;
            for(uint8_t _arrow = 0; _arrow < arrow_count; _arrow++) {
                const uint8_t arrow_index = buf_pop8(buffer);
                const uint8_t direction_and_flip = buf_pop8(buffer);
                arrow_t* arrow = &chunk->arrows[arrow_index];
                arrow->direction = direction_and_flip & 0b11;
                arrow->flipped = (direction_and_flip & 0b100) != 0;
                arrow->type = arrow_type;
            }
        }
    }
}

static inline void update_arrow(arrow_t* arrow, uint32_t* seed)
{
#define ar_random() ({ \
*seed = *seed * 1664525u + 1013904223u; \
*seed; \
})
    switch (arrow->type)
    {
    case AR_EMPTY:
    case AR_LEVEL_SOURCE:
    case AR_LEVEL_TARGET:
        break;
    case AR_ARROW:
    case AR_BLOCKER:
    case AR_SPLITTER_UP_DOWN:
    case AR_SPLITTER_UP_RIGHT:
    case AR_SPLITTER_UP_RIGHT_LEFT:
        if (arrow->signal_count > 0)
        {
            if (arrow->signal == S_NONE)
                arrow->signal = S_RED;
        } else {
            if (arrow->signal == S_RED)
                arrow->signal = S_NONE;
        }
        break;
    case AR_SOURCE:
        if (arrow->signal == S_NONE)
            arrow->signal = S_RED;
        break;
    case AR_DELAY:
        if (arrow->signal == S_BLUE) {
            arrow->signal = S_RED;
        } else if (arrow->signal_count > 0) {
            if (arrow->signal == S_NONE)
            {
                arrow->signal = S_BLUE;
            }
        } else if (arrow->signal != S_NONE) {
            arrow->signal = S_NONE;
        }
        break;
    case AR_DETECTOR:
        if (arrow->detector_signal != S_NONE)
        {
            if (arrow->signal == S_NONE)
                arrow->signal = S_RED;
        } else {
            if (arrow->signal == S_RED)
                arrow->signal = S_NONE;
        }
        break;
    case AR_IMPULSE:
        if (arrow->signal == S_NONE)
            arrow->signal = S_RED;
        else if (arrow->signal == S_RED)
            arrow->signal = S_BLUE;
        break;
    case AR_BLUE_ARROW:
    case AR_DIAGONAL_ARROW:
    case AR_BLUE_SPLITTER_UP_UP:
    case AR_BLUE_SPLITTER_RIGHT_UP:
    case AR_BLUE_SPLITTER_UP_DIAGONAL:
        if (arrow->signal_count > 0)
        {
            if (arrow->signal == S_NONE)
                arrow->signal = S_BLUE;
        } else {
            if (arrow->signal == S_BLUE)
                arrow->signal = S_NONE;
        }
        break;
    case AR_LOGIC_NOT:
        arrow->signal = (arrow->signal_count == 0) * 3;
        break;
    case AR_LOGIC_AND:
        arrow->signal = (arrow->signal_count > 1) * 3;
        break;
    case AR_LOGIC_XOR:
        arrow->signal = (arrow->signal_count % 2 == 1) * 3;
        break;
    case AR_LATCH:
        if (arrow->signal_count > 1)
            arrow->signal = S_YELLOW;
        else if (arrow->signal_count == 1)
            arrow->signal = S_NONE;
        break;
    case AR_FLIP_FLOP:
        if (arrow->signal_count > 0)
        {
            if (arrow->signal == S_NONE)
                arrow->signal = S_YELLOW;
            else
                arrow->signal = S_NONE;
        }
        break;
    case AR_RANDOM:
        if (arrow->signal_count > 0 && ar_random() % 128 > 63)
            arrow->signal = S_ORANGE;
        else
            arrow->signal = S_NONE;
        break;
    case AR_BUTTON:
        arrow->signal = S_NONE;
        break;
    case AR_DIRECTIONAL_BUTTON:
        arrow->signal = (arrow->signal_count > 0) * S_ORANGE;
        break;
    }
}

void map_update(map_t* map) {
    const size_t len = arrlen(map->changed_nodes);

    const int max_threads = omp_get_max_threads();
    arrow_t*** local_buffers = calloc(max_threads, sizeof(arrow_t**));

#pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        arrow_t** local = local_buffers[thread_id];

        #pragma omp for schedule(static)
        for (size_t i = 0; i < len; ++i)
        {
            arrow_t* arrow = map->changed_nodes[i];
            bool is_changed = arrow->signal != arrow->last_signal;
            if (is_changed)
            {
                if (!(arrow->type == AR_DELAY && arrow->signal == S_BLUE))
                {
                    const bool is_blocker = arrow->type == AR_BLOCKER;
                    const bool is_active = ACTIVE_SIGNALS[arrow->type] == arrow->signal;
                    for (int j = 0; j < arrow->edges_count; ++j)
                    {
                        arrow_t* edge = arrow->edges[j];
                        if (is_active) {
                            if (is_blocker) {
                                atomic_fetch_add_explicit(&edge->block_count, 1, memory_order_relaxed);
                            } else {
                                atomic_fetch_add_explicit(&edge->signal_count, 1, memory_order_relaxed);
                            }
                        } else {// TODO: may be just fetch_add is_active ? -1 : 1 ???
                            if (is_blocker) {
                                atomic_fetch_sub_explicit(&edge->block_count, 1, memory_order_relaxed);
                            } else {
                                atomic_fetch_sub_explicit(&edge->signal_count, 1, memory_order_relaxed);
                            }
                        }
                        if (atomic_exchange_explicit(&edge->changed, 1, memory_order_relaxed) == 0)
                        {
                            arrpush(local, edge);
                        }
                    }
                }
                for (int j = 0; j < arrow->detectors_count; ++j)
                {
                    arrow_t* detector = arrow->detectors[j];
                    detector->detector_signal = arrow->signal;
                    if (atomic_exchange_explicit(&detector->changed, 1, memory_order_relaxed) == 0)
                    {
                        arrpush(local, detector);
                    }
                }
            }
            // TIP: Bruh
            if ((is_changed && is_additional_update(arrow->type)) ||
                (map->restarted && is_entry_point(arrow->type)) ||
                (arrow->signal != S_NONE && (arrow->type == AR_BUTTON || (arrow->signal_count == 0 && arrow->type == AR_DIRECTIONAL_BUTTON))) ||
                (arrow->type == AR_RANDOM && arrow->signal_count > 0))
            {
                if (atomic_exchange_explicit(&arrow->changed, 1, memory_order_relaxed) == 0)
                    arrpush(local, arrow);
            }

            arrow->last_signal = arrow->signal;
        }

        local_buffers[thread_id] = local;
    }

    size_t total_size = 0;
    size_t* offsets = calloc(max_threads, sizeof(size_t));
    for (size_t i = 0; i < max_threads; ++i)
    {
        offsets[i] = total_size;
        total_size += arrlen(local_buffers[i]);
    }

    arrow_t** new_changed_nodes = map->temp_changed_nodes;
    if (arrcap(new_changed_nodes) < total_size) {
        arrsetcap(new_changed_nodes, total_size);
    }
    arrsetlen(new_changed_nodes, total_size);

#pragma omp parallel for
    for (size_t i = 0; i < max_threads; ++i)
    {
#ifdef DEBUG
        if (local_buffers[i] == NULL) {
            continue;
        }
#endif
        const size_t len = arrlen(local_buffers[i]);
        const size_t offset = offsets[i];
        memcpy(new_changed_nodes + offset, local_buffers[i], len * sizeof(arrow_t*));
        arrfree(local_buffers[i]);
    }

    const uint32_t start_seed = rand(); // NOLINT(*-msc50-cpp)
#pragma omp parallel
    {
        uint32_t seed = start_seed + 123u * omp_get_thread_num();
        #pragma omp for schedule(static)
        for (size_t i = 0; i < arrlen(new_changed_nodes); ++i)
        {
            arrow_t* arrow = new_changed_nodes[i];
            arrow->changed = false;
            // TODO: Compare (bool * signal) and if
            if (arrow->block_count > 0)
                arrow->signal = S_NONE;
            else
                update_arrow(arrow, &seed);
        }
    }

    map->temp_changed_nodes = map->changed_nodes;
    map->changed_nodes = new_changed_nodes;
    map->restarted = false;

    free(local_buffers);
    free(offsets);
}

void map_update_single(map_t* map) {
    arrow_t** temp_buffer = map->temp_changed_nodes;
    arrsetlen(temp_buffer, 0);

    const size_t len = arrlen(map->changed_nodes);
    for (size_t i = 0; i < len; ++i)
    {
        arrow_t* arrow = map->changed_nodes[i];
        const bool is_changed = arrow->signal != arrow->last_signal;
        if (is_changed)
        {
            if (!(arrow->type == AR_DELAY && arrow->signal == S_BLUE))
            {
                const bool is_blocker = arrow->type == AR_BLOCKER;
                const bool is_active = ACTIVE_SIGNALS[arrow->type] == arrow->signal;
                const int delta = is_active ? 1 : -1;
                for (int j = 0; j < arrow->edges_count; ++j)
                {
                    arrow_t* edge = arrow->edges[j];
                    uint8_t* target_count = is_blocker ? &edge->block_count : &edge->signal_count;
                    *target_count += delta;
                    if (!edge->changed)
                    {
                        arrpush(temp_buffer, edge);
                        edge->changed = true;
                    }
                }
            }
            for (int j = 0; j < arrow->detectors_count; ++j)
            {
                arrow_t* detector = arrow->detectors[j];
                detector->detector_signal = arrow->signal;
                if (!detector->changed)
                {
                    arrpush(temp_buffer, detector);
                    detector->changed = true;
                }
            }
        }
        // TIP: Bruh
        if (((is_changed && is_additional_update(arrow->type)) ||
            (map->restarted && is_entry_point(arrow->type)) ||
            (arrow->signal != S_NONE && (arrow->type == AR_BUTTON || (arrow->signal_count == 0 && arrow->type == AR_DIRECTIONAL_BUTTON))) ||
            (arrow->type == AR_RANDOM && arrow->signal_count > 0)) && !arrow->changed)
        {
            arrpush(temp_buffer, arrow);
            arrow->changed = true;
        }
        arrow->last_signal = arrow->signal;
    }

    uint32_t seed = rand();
    size_t buf_len = arrlen(temp_buffer);
    for (size_t i = 0; i < buf_len; ++i)
    {
        arrow_t* arrow = temp_buffer[i];
        arrow->changed = false;
        // TODO: Compare (bool * signal) and if
        if (arrow->block_count > 0)
            arrow->signal = S_NONE;
        else
            update_arrow(arrow, &seed);
    }

    map->temp_changed_nodes = map->changed_nodes;
    map->changed_nodes = temp_buffer;
    map->restarted = false;
}

void map_import(map_t* map, const char* input) {
    buffer_t buffer = { 0 };
    buf_base64_decode(&buffer, input);
    assert(buffer.data != NULL && "Error in base64 decoding");
    map_load(map, &buffer);
    free(buffer.data);
    map_recompile(map);
}
