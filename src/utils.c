#include "SDL.h"
#include "utils.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void die(const char *fmt, ...)
{
    va_list vargs;

    va_start(vargs, fmt);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, fmt,
                    vargs);
    va_end(vargs);

    exit(EXIT_FAILURE);
}

void *xmalloc(size_t size)
{
    void *p = malloc(size);
    if (!p)
        die("malloc: %s", strerror(errno));
    return p;
}

void *xrealloc(void *ptr, size_t size)
{
    void *p = realloc(ptr, size);
    if (!p)
        die("realloc: %s", strerror(errno));
    return p;
}

char *readline(FILE *stream, const char eolChar, size_t startingSize,
                size_t scalingFactor)
{
    if (!startingSize)
        startingSize = UTILS_READLINE_DEFAULT_STARTING_SIZE;

    if (!scalingFactor)
        scalingFactor = UTILS_READLINE_DEFAULT_SCALING_FACTOR;

    size_t len = 0;
    size_t size = startingSize;
    char ch = 0;
    char *str = (char *)malloc(sizeof(char) * startingSize);

    while ((ch = fgetc(stream)) != EOF && ch != eolChar)
    {
        str[len] = ch;

        len++;

        if (len == size)
            str = xrealloc(str, size *= scalingFactor);
    }

    str[len] = '\0';

    return (char *)realloc(str, sizeof(char) * (len + 1));
}
