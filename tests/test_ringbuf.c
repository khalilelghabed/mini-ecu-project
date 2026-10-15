#include <stdio.h>
#include "app/ringbuf.h"

#define CHECK(c) do { if (!(c)) { \
    printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); return 1; } } while (0)

static int test_fifo_order(void)
{
    uint8_t storage[8];
    ringbuf_t rb;
    uint8_t v;
    ringbuf_init(&rb, storage, sizeof storage);

    for (uint8_t i = 1; i <= 5; i++) {
        CHECK(ringbuf_put(&rb, i));
    }
    CHECK(ringbuf_count(&rb) == 5);
    for (uint8_t i = 1; i <= 5; i++) {
        CHECK(ringbuf_get(&rb, &v));
        CHECK(v == i);
    }
    return 0;
}

static int test_empty(void)
{
    uint8_t storage[4];
    ringbuf_t rb;
    uint8_t v = 99;
    ringbuf_init(&rb, storage, sizeof storage);

    CHECK(ringbuf_is_empty(&rb));
    CHECK(!ringbuf_get(&rb, &v));
    CHECK(v == 99);
    return 0;
}

static int test_full_and_wrap(void)
{
    uint8_t storage[4];            /* capacité utile : 3 */
    ringbuf_t rb;
    uint8_t v;
    ringbuf_init(&rb, storage, sizeof storage);

    CHECK(ringbuf_put(&rb, 10));
    CHECK(ringbuf_put(&rb, 11));
    CHECK(ringbuf_put(&rb, 12));
    CHECK(ringbuf_is_full(&rb));
    CHECK(!ringbuf_put(&rb, 13));

    CHECK(ringbuf_get(&rb, &v));
    CHECK(v == 10);
    CHECK(ringbuf_put(&rb, 13));   /* l'écriture repasse au début du tableau */

    CHECK(ringbuf_get(&rb, &v)); CHECK(v == 11);
    CHECK(ringbuf_get(&rb, &v)); CHECK(v == 12);
    CHECK(ringbuf_get(&rb, &v)); CHECK(v == 13);
    CHECK(ringbuf_is_empty(&rb));
    return 0;
}

int main(void)
{
    int fails = 0;
    fails += test_fifo_order();
    fails += test_empty();
    fails += test_full_and_wrap();
    if (fails == 0) {
        printf("OK : 3 tests reussis\n");
    }
    return fails;
}
