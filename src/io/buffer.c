#include "../../include/io/buffer.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static const char base64_enc_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const int8_t base64_dec_table[256] = {
    ['A']=0, ['B']=1, ['C']=2, ['D']=3, ['E']=4, ['F']=5, ['G']=6, ['H']=7,
    ['I']=8, ['J']=9, ['K']=10, ['L']=11, ['M']=12, ['N']=13, ['O']=14, ['P']=15,
    ['Q']=16, ['R']=17, ['S']=18, ['T']=19, ['U']=20, ['V']=21, ['W']=22, ['X']=23,
    ['Y']=24, ['Z']=25, ['a']=26, ['b']=27, ['c']=28, ['d']=29, ['e']=30, ['f']=31,
    ['g']=32, ['h']=33, ['i']=34, ['j']=35, ['k']=36, ['l']=37, ['m']=38, ['n']=39,
    ['o']=40, ['p']=41, ['q']=42, ['r']=43, ['s']=44, ['t']=45, ['u']=46, ['v']=47,
    ['w']=48, ['x']=49, ['y']=50, ['z']=51, ['0']=52, ['1']=53, ['2']=54, ['3']=55,
    ['4']=56, ['5']=57, ['6']=58, ['7']=59, ['8']=60, ['9']=61, ['+']=62, ['/']=63
};

char* buf_base64_encode(const buffer_t* buffer, size_t* out_len) {
    size_t len = buffer->size;
    size_t olen = 4 * ((len + 2) / 3);
    char* out = malloc(olen + 1);
    if (!out) return NULL;

    size_t j = 0;
    for (size_t i = 0; i < len; i += 3) {
        uint32_t a = buffer->data[i];
        uint32_t b = (i + 1 < len) ? buffer->data[i + 1] : 0;
        uint32_t c = (i + 2 < len) ? buffer->data[i + 2] : 0;
        uint32_t triple = (a << 16) | (b << 8) | c;

        out[j++] = base64_enc_table[(triple >> 18) & 0x3F];
        out[j++] = base64_enc_table[(triple >> 12) & 0x3F];
        out[j++] = (i + 1 < len) ? base64_enc_table[(triple >> 6) & 0x3F] : '=';
        out[j++] = (i + 2 < len) ? base64_enc_table[triple & 0x3F] : '=';
    }

    out[j] = '\0';
    if (out_len) *out_len = j;
    return out;
}

int buf_base64_decode(buffer_t* out_buffer, const char* input) {
    size_t len = strlen(input);
    if (len % 4 != 0) {
        out_buffer->data = NULL;
        out_buffer->size = 0;
        return -1;
    }

    size_t out_capacity = (len / 4) * 3;
    uint8_t* out = malloc(out_capacity);
    if (!out) {
        out_buffer->data = NULL;
        out_buffer->size = 0;
        return -1;
    }

    size_t out_size = 0;
    for (size_t i = 0; i < len; i += 4) {
        int pad = 0;
        int8_t s[4];
        for (int j = 0; j < 4; j++) {
            char c = input[i + j];
            if (c == '=') {
                s[j] = 0;
                pad++;
            } else {
                int8_t v = base64_dec_table[(unsigned char)c];
                if (v < 0 || (v == 0 && c != 'A')) {
                    free(out);
                    out_buffer->data = NULL;
                    out_buffer->size = 0;
                    return -1;
                }
                s[j] = v;
            }
        }

        uint32_t triple = (s[0] << 18) | (s[1] << 12) | (s[2] << 6) | s[3];
        if (pad < 3) out[out_size++] = (triple >> 16) & 0xFF;
        if (pad < 2) out[out_size++] = (triple >> 8) & 0xFF;
        if (pad < 1) out[out_size++] = triple & 0xFF;
    }

    out_buffer->data = out;
    out_buffer->size = out_size;
    return 0;
}

uint8_t buf_pop8(buffer_t* buffer) {
    assert(buffer->cursor < buffer->size && "Buffer corrupted, out of bounds pop8");
    return buffer->data[buffer->cursor++];
}

uint16_t buf_pop16(buffer_t* buffer) {
    assert(buffer->cursor + 1 < buffer->size && "Buffer corrupted, out of bounds pop16");
    const uint16_t value = *(uint16_t*) (buffer->data + buffer->cursor);
    buffer->cursor += 2;
    return value;
}