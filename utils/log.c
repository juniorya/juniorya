#include "utils/log.h"

#include "osal/osal.h"

#include <stdarg.h>

static const char *level_names[] = {"DEBUG", "INFO", "WARN", "ERROR"};

void log_init(void)
{
    /* No-op for the portable build. */
}

void log_printf(log_level level, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    uint64_t ts = osal_clock_monotonic_ns();
    fprintf(stdout, "[%010llu] %s: ", (unsigned long long)(ts / 1000ULL), level_names[level]);
    vfprintf(stdout, fmt, args);
    fputc('\n', stdout);
    va_end(args);
}
