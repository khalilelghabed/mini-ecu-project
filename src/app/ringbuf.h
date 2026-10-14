#ifndef RINGBUF_H
#define RINGBUF_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t *buf;
    uint16_t size;
    uint16_t head;
    uint16_t tail;
} ringbuf_t;

void     ringbuf_init(ringbuf_t *rb, uint8_t *storage, uint16_t size);
bool     ringbuf_put(ringbuf_t *rb, uint8_t byte);
bool     ringbuf_get(ringbuf_t *rb, uint8_t *byte);
bool     ringbuf_is_empty(const ringbuf_t *rb);
bool     ringbuf_is_full(const ringbuf_t *rb);
uint16_t ringbuf_count(const ringbuf_t *rb);

#endif /* RINGBUF_H */