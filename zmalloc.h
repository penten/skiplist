#include <stdlib.h>

void *zmalloc(size_t size);
void *zrealloc(void *mem, size_t size);

static void zerror();
