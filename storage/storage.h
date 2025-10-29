/**
 * @file storage.h
 * @brief CRC-protected key-value storage for calibration parameters.
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>
#include <stddef.h>

#include "utils/q16.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Storage initialisation options. */
typedef struct
{
    const char *path_primary; /**< Primary storage file. */
    const char *path_backup;  /**< Backup file. */
} storage_init_options;

/**
 * @brief Fill storage options with defaults.
 * @param opts Options structure.
 */
void storage_default_options(storage_init_options *opts);

/**
 * @brief Initialise storage backend.
 * @param opts Storage options.
 * @return True on success.
 */
bool storage_init(const storage_init_options *opts);

/**
 * @brief Persist calibration payload.
 * @param data Pointer to payload.
 * @param size Payload size in bytes.
 * @return True on success.
 */
bool storage_write(const void *data, size_t size);

/**
 * @brief Load calibration payload.
 * @param data Output buffer.
 * @param size Buffer size.
 * @return True if valid record exists.
 */
bool storage_read(void *data, size_t size);

/**
 * @brief Shutdown storage.
 */
void storage_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
