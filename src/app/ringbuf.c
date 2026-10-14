
#include "ringbuf.h"

void ringbuf_init(ringbuf_t *rb, uint8_t *storage, uint16_t size)
{
    rb->buf = storage;
    rb->size = size;
    rb->head = 0;
    rb->tail = 0;
}

bool ringbuf_is_empty(const ringbuf_t *rb)
{
    return (rb->head == rb->tail);
}

bool ringbuf_is_full(const ringbuf_t *rb)
{
    return ((rb->head + 1) % rb->size == rb->tail);
}

bool ringbuf_put(ringbuf_t *rb, uint8_t byte)
{
    if (ringbuf_is_full(rb)) {
        return false;
    }
    rb->buf[rb->head] = byte;
    rb->head = (rb->head + 1) % rb->size;
    return true;
}

bool ringbuf_get(ringbuf_t *rb, uint8_t *byte)
{
    if (ringbuf_is_empty(rb)) {
        return false;
    }

    *byte = rb->buf[rb->tail];

    rb->tail = (rb->tail + 1) % rb->size;

    return true;
}




uint16_t ringbuf_count(const ringbuf_t *rb)
{
    if (rb->head >= rb->tail) {
        return rb->head - rb->tail;
    }
    else {
        return rb->size - rb->tail + rb->head;
    }
}