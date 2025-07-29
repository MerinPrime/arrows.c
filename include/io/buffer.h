#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

typedef struct
{
    size_t cursor;
    size_t size;
    uint8_t* data;
} buffer_t;

char* buf_base64_encode(const buffer_t* buffer, size_t* out_len);
int buf_base64_decode(buffer_t* out_buffer, const char* input);

uint8_t buf_pop8(buffer_t* buffer);
uint16_t buf_pop16(buffer_t* buffer);

#endif
