/**
 * @file osal.h
 * @brief Operating system abstraction layer for threading, timing and I/O.
 */

#ifndef OSAL_H
#define OSAL_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*osal_thread_fn)(void *arg);

typedef struct osal_mutex osal_mutex;
typedef struct osal_event osal_event;
typedef struct osal_thread osal_thread;

/**
 * @brief Initialise OS abstraction layer.
 * @return True on success.
 */
bool osal_init(void);

/**
 * @brief Shut down OS abstraction layer.
 */
void osal_shutdown(void);

/**
 * @brief Create and start real-time thread.
 * @param fn Thread function.
 * @param arg User argument.
 * @param priority Relative priority (0 = normal, higher = more critical).
 * @return Thread handle or NULL on failure.
 */
osal_thread *osal_thread_create(osal_thread_fn fn, void *arg, int priority);

/**
 * @brief Wait for thread completion.
 * @param thread Thread handle.
 */
void osal_thread_join(osal_thread *thread);

/**
 * @brief Destroy thread resources.
 * @param thread Thread handle.
 */
void osal_thread_destroy(osal_thread *thread);

/**
 * @brief Allocate mutex.
 * @return Mutex handle or NULL.
 */
osal_mutex *osal_mutex_create(void);

/**
 * @brief Lock mutex.
 * @param mutex Mutex handle.
 */
void osal_mutex_lock(osal_mutex *mutex);

/**
 * @brief Unlock mutex.
 * @param mutex Mutex handle.
 */
void osal_mutex_unlock(osal_mutex *mutex);

/**
 * @brief Destroy mutex resources.
 * @param mutex Mutex handle.
 */
void osal_mutex_destroy(osal_mutex *mutex);

/**
 * @brief Create auto-reset event object.
 * @return Event handle or NULL.
 */
osal_event *osal_event_create(void);

/**
 * @brief Wait for event signal with timeout.
 * @param event Event handle.
 * @param timeout_ms Timeout in milliseconds.
 * @return True if event was signaled.
 */
bool osal_event_wait(osal_event *event, uint32_t timeout_ms);

/**
 * @brief Signal event.
 * @param event Event handle.
 */
void osal_event_signal(osal_event *event);

/**
 * @brief Destroy event object.
 * @param event Event handle.
 */
void osal_event_destroy(osal_event *event);

/**
 * @brief Sleep for specified milliseconds.
 * @param ms Duration in milliseconds.
 */
void osal_sleep_ms(uint32_t ms);

/**
 * @brief Return monotonic timestamp in microseconds.
 * @return Timestamp value.
 */
uint64_t osal_time_us(void);

#endif
