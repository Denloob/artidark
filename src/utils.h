#include <stdlib.h>
#include <stdio.h>

#define UTILS_READLINE_DEFAULT_STARTING_SIZE 16
#define UTILS_READLINE_DEFAULT_SCALING_FACTOR 2

/**
 * @brief Logs the given message using SDL and exits with EXIT_FAILURE
 */
void die(const char *fmt, ...);

/**
 * @brief Allocates memory and calls `die` on failure.
 *
 * @param size The size of the memory to allocate.
 * @return Pointer to the allocated memory.
 */
void *xmalloc(size_t size);

/**
 * @brief Reallocates `ptr` and calls `die` on failure.
 *
 * @param ptr The pointer to reallocate.
 * @param size The size of the memory to reallocate.
 * @return Pointer to the reallocated memory.
 */
void *xrealloc(void *ptr, size_t size);

/**
 * @brief Reads a line from the given stream up until the given eol character.
 *
 *
 * @param stream The stream to read from.
 * @param eolChar The character up until which to read.
 *                  (Is not included in the result string).
 * @param startingSize The starting size of the buffer. If 0, the default is
 *                      used.
 * @param scalingFactor The scaling factor to apply to the buffer. If 0, the
 *                       default is used.
 * @return Dynamicly allocated and null terminated string which was read.
 */
char *readline(FILE *stream, const char eolChar, size_t startingSize,
                size_t scalingFactor);
