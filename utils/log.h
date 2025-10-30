/**
 * @file log.h
 * @brief Lightweight logging facility with compile-time severity levels.
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Severity levels for logging output. */
typedef enum
{
    LOG_TRACE = 0, /**< Trace-level diagnostics. */
    LOG_INFO,      /**< Informational messages. */
    LOG_WARN,      /**< Warnings about recoverable issues. */
    LOG_ERROR      /**< Fatal errors. */
} log_severity;

/**
 * @brief Configure logging subsystem.
 * @param min_severity Minimum severity to emit.
 */
void log_init(log_severity min_severity);

/**
 * @brief Emit formatted log message.
 * @param severity Message severity.
 * @param fmt printf-style format string.
 */
void log_printf(log_severity severity, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
