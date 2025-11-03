#ifndef LOG_H
#define LOG_H

#include <stdio.h>

/** \brief Log severity levels. */
typedef enum
{
    LOG_DEBUG, /**< Debug information. */
    LOG_INFO,  /**< Informational message. */
    LOG_WARN,  /**< Warning condition. */
    LOG_ERROR  /**< Error condition. */
} log_level;

/** \brief Initialise logging backend. */
void log_init(void);

/** \brief Log a formatted message. */
void log_printf(log_level level, const char *fmt, ...);

#endif
