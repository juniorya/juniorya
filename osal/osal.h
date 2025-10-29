/**
 * @file osal.h
 * @brief Operating system abstraction layer used to target multiple RTOSes.
 *
 * The OSAL exposes threads, mutexes, events and timers in a minimal interface to
 * keep the core motion control code portable across QNX, VxWorks, Baget and the
 * POSIX host simulation. All functions return boolean success values and avoid
 * dynamic allocations in the control path.
 */
#ifndef OSAL_OSAL_H
#define OSAL_OSAL_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*osal_thread_fn)(void *arg);

typedef struct
{
    void *impl;
} osal_thread_t;

typedef struct
{
    void *impl;
} osal_mutex_t;

typedef struct
{
    void *impl;
} osal_event_t;

/**
 * @brief Initialize the OSAL runtime.
 *
 * @return true on success.
 */
bool osal_init(void);

/**
 * @brief Create a thread with provided entry function.
 *
 * @param thread Thread object to initialize.
 * @param fn Entry point.
 * @param arg Argument passed to entry function.
 * @param priority Scheduling priority (0 = default).
 * @return true on success.
 */
bool osal_thread_create(osal_thread_t *thread,
                        osal_thread_fn fn,
                        void *arg,
                        int priority);

/**
 * @brief Join a thread and release resources.
 *
 * @param thread Thread to join.
 */
void osal_thread_join(osal_thread_t *thread);

/**
 * @brief Initialize a mutex.
 *
 * @param mutex Mutex object.
 * @return true on success.
 */
bool osal_mutex_init(osal_mutex_t *mutex);

/**
 * @brief Lock a mutex.
 *
 * @param mutex Mutex object.
 */
void osal_mutex_lock(osal_mutex_t *mutex);

/**
 * @brief Unlock a mutex.
 *
 * @param mutex Mutex object.
 */
void osal_mutex_unlock(osal_mutex_t *mutex);

/**
 * @brief Initialize an auto-reset event.
 *
 * @param event Event object.
 * @return true on success.
 */
bool osal_event_init(osal_event_t *event);

/**
 * @brief Signal an event.
 *
 * @param event Event object.
 */
void osal_event_signal(osal_event_t *event);

/**
 * @brief Wait for an event with timeout.
 *
 * @param event Event object.
 * @param timeout_ms Timeout in milliseconds.
 * @return true if the event was signaled.
 */
bool osal_event_wait(osal_event_t *event, uint32_t timeout_ms);

/**
 * @brief Sleep for the specified number of milliseconds.
 *
 * @param duration_ms Duration to sleep.
 */
void osal_sleep_ms(uint32_t duration_ms);

#endif /* OSAL_OSAL_H */
