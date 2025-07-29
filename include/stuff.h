#ifndef STUFF_H
#define STUFF_H

#define arrfind(arr, value) \
    ({                                                              \
        ptrdiff_t idx = -1;                                         \
        for (ptrdiff_t i = 0; i < arrlen(arr); i++) {               \
            if ((arr)[i] == (value)) { idx = i; break; }            \
        }                                                           \
        idx;                                                        \
    })

#endif
