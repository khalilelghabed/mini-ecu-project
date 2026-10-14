#!/bin/bash
mkdir -p build && gcc -Wall -Wextra -Werror -std=c11 -Isrc/app src/app/ringbuf.c tests/test_ringbuf.c -o build/test_ringbuf && ./build/test_ringbuf
