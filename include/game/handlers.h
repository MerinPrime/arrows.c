#ifndef HANDLERS_H
#define HANDLERS_H

#include "signal_type.h"
#include <stdbool.h>

#include "arrow_type.h"

#define MAX_EDGES 4
#define MAX_DETECTORS 4

extern const SignalType ACTIVE_SIGNALS[];
extern const bool NOT_ALLOWED_TO_CHANGE_TABLE[];
extern const bool ENTRY_POINT_TABLE[];
extern const bool ADDITIONAL_UPDATE_TABLE[];

inline bool is_not_allowed_to_change(const ArrowType arrow_type) {
    return NOT_ALLOWED_TO_CHANGE_TABLE[arrow_type];
}

inline bool is_entry_point(const ArrowType arrow_type) {
    return ENTRY_POINT_TABLE[arrow_type];
}

inline bool is_additional_update(const ArrowType arrow_type) {
    return ADDITIONAL_UPDATE_TABLE[arrow_type];
}

#endif
