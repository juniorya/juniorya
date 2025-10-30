/**
 * @file log.c
 * @brief Minimal logging facility implementation.
 */

#include "log.h"

#include <stdio.h>
#include <time.h>

static log_severity g_min_severity = LOG_INFO;

void log_init(log_severity min_severity)
{
    g_min_severity = min_severity;
}

static const char *severity_str(log_severity severity)
{
    switch (severity)
    {
        case LOG_TRACE:
            return "TRACE";
        case LOG_INFO:
            return "INFO";
        case LOG_WARN:
            return "WARN";
        case LOG_ERROR:
        default:
            return "ERROR";
    }
}

void log_printf(log_severity severity, const char *fmt, ...)
{
    if (severity < g_min_severity)
    {
        return;
    }
    time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);
    char buf[32];
    if (tm_now != NULL)
    {
        (void)strftime(buf, sizeof(buf), "%H:%M:%S", tm_now);
    }
    else
    {
        buf[0] = '\0';
    }

    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "%s [%s] ", buf, severity_str(severity));
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}
