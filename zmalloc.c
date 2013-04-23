#include <stdlib.h>
#include <stdio.h>
#include "zmalloc.h"

void *zmalloc(size_t size) {
    void *mem;

    mem = malloc(size);
    if(!mem) zerror();
    return mem;
}

void *zrealloc(void *mem, size_t size) {
    mem = realloc(mem, size);
    if(!mem) zerror();

    return mem;
}

static void zerror(void) {
    fprintf(stderr, "Out of memory");
    fflush(stderr);
    exit(1);
}
