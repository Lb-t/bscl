#pragma once
#include <stdint.h>
#include <string.h>

typedef uint8_t buffer_data_type_t;

typedef struct {
    buffer_data_type_t *buf; // main buffer
    uint32_t size;           // MUST BE 2^n
    uint32_t wrp;
    uint32_t rdp;
} buffer_t;

#define BUFFER_TYPE_SIZE sizeof(buffer_data_type_t)

#define buffer_offset_of_pos(buffer, pos) ((pos) & ((buffer)->size - 1))
#define buffer_addr_of_pos(buffer, pos) ((buffer)->buf + buffer_offset_of_pos(buffer, pos))

#define buffer_write_addr(buffer) buffer_addr_of_pos(buffer, (buffer)->wrp)
#define buffer_read_addr(buffer) buffer_addr_of_pos(buffer, (buffer)->rdp)

// size :number of uint32_t
static inline void buffer_init(buffer_t *buffer, buffer_data_type_t *mem_buf, uint32_t size)
{
    buffer->buf = mem_buf;
    buffer->size = size;
    buffer->wrp = 0;
    buffer->rdp = 0;
}

static inline void buffer_clean(buffer_t *buffer)
{
    buffer->rdp = buffer->wrp;
}

static inline uint32_t buffer_get_count(buffer_t *buffer)
{
    return buffer->wrp - buffer->rdp;
}

static inline uint32_t buffer_get_space(buffer_t *buffer)
{
    return buffer->size - buffer_get_count(buffer);
}

static inline uint32_t buffer_get_size(buffer_t *buffer)
{
    return buffer->size;
}

static inline int buffer_is_empty(buffer_t *buffer)
{
    return buffer->wrp == buffer->rdp;
}

static inline int buffer_is_full(buffer_t *buffer)
{
    return buffer->wrp - buffer->rdp >= buffer->size;
}

static inline void _buffer_wrp_add(buffer_t *buffer, int n)
{
    buffer->wrp += n;
}

static inline void _buffer_rdp_add(buffer_t *buffer, int n)
{
    buffer->rdp += n;
}

// return writed number of uint32_t, -1 for error
static inline int buffer_write(buffer_t *buffer, const void *data, uint32_t len)
{
    uint32_t space = buffer_get_space(buffer);
    len = len < space ? len : space;
    if (len) {
        uint32_t offset = buffer_offset_of_pos(buffer, buffer->wrp);
        if ((offset + len) <= buffer->size) {
            memcpy(buffer->buf + offset, data, len * BUFFER_TYPE_SIZE);
        } else {
            uint32_t left = buffer->size - offset;
            memcpy(buffer->buf + offset, data, left * BUFFER_TYPE_SIZE);
            memcpy(buffer->buf, data + left, (len - left) * BUFFER_TYPE_SIZE);
        }
        buffer->wrp += len;
    }
    return len;
}

static inline int buffer_putc(buffer_t *buffer, buffer_data_type_t c)
{

    if (buffer_is_full(buffer)) {
        return 0;
    } else {
        uint32_t offset = buffer_offset_of_pos(buffer, buffer->wrp);
        buffer->buf[offset] = c;
        buffer->wrp++;
        return 1;
    }
}

static inline int buffer_copy_char(buffer_t *buffer, uint32_t begin)
{
    uint32_t count = buffer_get_count(buffer);
    if (count < begin) {
        return -1;
    }
    uint32_t offset = buffer_offset_of_pos(buffer, buffer->rdp + begin);
    return buffer->buf[offset];
}

// return uint32_t read out
static inline int buffer_copy(buffer_t *buffer, uint32_t begin, void *data, uint32_t len)
{
    uint32_t count = buffer_get_count(buffer);
    if (count < begin) {
        return 0;
    }

    if (count < len + begin) {
        len = count - begin;
    }

    if (len) {
        uint32_t offset = buffer_offset_of_pos(buffer, buffer->rdp + begin);
        if ((offset + len) <= buffer->size) {
            memcpy(data, buffer->buf + offset, len * BUFFER_TYPE_SIZE);
        } else {
            uint32_t left = buffer->size - offset;
            memcpy(data, buffer->buf + offset, left * BUFFER_TYPE_SIZE);
            memcpy(data + left, buffer->buf, (len - left) * BUFFER_TYPE_SIZE);
        }
    }
    return len;
}

// return words read out
static inline int buffer_read(buffer_t *buffer, buffer_data_type_t *buf, uint32_t size)
{
    int n = buffer_copy(buffer, 0, buf, size);

    _buffer_rdp_add(buffer, n);

    return n;
}

// callee must ensure i is in range
static inline buffer_data_type_t buffer_copy_word(buffer_t *buffer, uint32_t i)
{
    uint32_t offset = buffer_offset_of_pos(buffer, buffer->rdp + i);
    return buffer->buf[offset];
}

static inline int buffer_copy_first(buffer_t *buffer)
{
    return buffer_copy_word(buffer, 0);
}

// return word dropped
static inline int buffer_drop(buffer_t *buffer, uint32_t len)
{
    int count = buffer_get_count(buffer);
    if (count < len) {
        len = count;
    }
    _buffer_rdp_add(buffer, len);
    return len;
}

static inline int buffer_drop_char(buffer_t *buffer)
{
    return buffer_drop(buffer, 1);
}