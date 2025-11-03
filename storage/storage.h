#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** \brief Storage configuration options. */
typedef struct
{
    const char *path; /**< Path to storage file or device. */
} storage_options;

/** \brief Initialise storage subsystem. */
bool storage_init(const storage_options *options);

/** \brief Shutdown storage subsystem. */
void storage_shutdown(void);

/** \brief Default options for storage. */
void storage_default_options(storage_options *options);

/** \brief Store key-value pair. */
bool storage_write(const char *key, const void *data, size_t length);

/** \brief Read key-value pair. */
bool storage_read(const char *key, void *data, size_t length);

#endif
