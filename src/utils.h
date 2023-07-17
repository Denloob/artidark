#include <stdlib.h>

/**
 * @brief Logs the given message using SDL and exits with EXIT_FAILURE
 */
void die(const char *fmt, ...);

/**
 * @brief Allocates memory and calls `die` on failure.
 *
 * @param size The size of the memory to allocate.
 */
void *xmalloc(size_t size);