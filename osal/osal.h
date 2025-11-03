#ifndef OSAL_H
#define OSAL_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/** \brief Thread entry prototype. */
typedef void (*osal_thread_entry)(void *);

/** \brief Timer callback prototype. */
typedef void (*osal_timer_cb)(void *);

/** \brief Opaque thread handle. */
typedef struct osal_thread
{
    void *handle; /**< Platform handle. */
} osal_thread;

/** \brief Mutex wrapper. */
typedef struct osal_mutex
{
    void *handle; /**< Platform handle. */
} osal_mutex;

/** \brief Event primitive. */
typedef struct osal_event
{
    void *handle; /**< Platform handle. */
} osal_event;

/** \brief Periodic timer descriptor. */
typedef struct osal_timer
{
    void *handle; /**< Platform handle. */
} osal_timer;

/** \brief UDP socket abstraction. */
typedef struct osal_socket
{
    int fd; /**< File descriptor. */
} osal_socket;

/** \brief Process class determines scheduling priority. */
typedef enum
{
    OSAL_PROCESS_CLASS_REALTIME, /**< Real-time control thread. */
    OSAL_PROCESS_CLASS_AUXILIARY /**< Auxiliary services. */
} osal_process_class;

/** \brief Initialise the OSAL runtime. */
bool osal_init(void);

/** \brief Shutdown the OSAL runtime. */
void osal_shutdown(void);

/** \brief Create a thread with specific priority. */
bool osal_thread_create(osal_thread *thread,
                        osal_thread_entry entry,
                        void *arg,
                        osal_process_class cls,
                        const char *name);

/** \brief Join a thread. */
void osal_thread_join(osal_thread *thread);

/** \brief Destroy a thread handle. */
void osal_thread_destroy(osal_thread *thread);

/** \brief Initialise a mutex. */
bool osal_mutex_init(osal_mutex *mutex);

/** \brief Lock a mutex. */
void osal_mutex_lock(osal_mutex *mutex);

/** \brief Unlock a mutex. */
void osal_mutex_unlock(osal_mutex *mutex);

/** \brief Destroy mutex. */
void osal_mutex_destroy(osal_mutex *mutex);

/** \brief Initialise an event. */
bool osal_event_init(osal_event *event);

/** \brief Signal event. */
void osal_event_signal(osal_event *event);

/** \brief Wait for event with timeout in milliseconds. */
bool osal_event_wait(osal_event *event, uint32_t timeout_ms);

/** \brief Destroy event. */
void osal_event_destroy(osal_event *event);

/** \brief Start a periodic timer. */
bool osal_timer_start(osal_timer *timer,
                      uint32_t period_us,
                      osal_timer_cb cb,
                      void *arg);

/** \brief Stop periodic timer. */
void osal_timer_stop(osal_timer *timer);

/** \brief Sleep for specified milliseconds. */
void osal_sleep_ms(uint32_t ms);

/** \brief Retrieve monotonic clock in nanoseconds. */
uint64_t osal_clock_monotonic_ns(void);

/** \brief Set CPU affinity to core index. */
bool osal_affinity_set(osal_process_class cls, uint32_t core_index);

/** \brief Open UDP socket bound to port. */
bool osal_socket_udp_open(osal_socket *socket, uint16_t port);

/** \brief Close socket. */
void osal_socket_close(osal_socket *socket);

/** \brief Send UDP packet. */
int32_t osal_socket_udp_send(osal_socket *socket,
                             const void *data,
                             size_t length,
                             const char *address,
                             uint16_t port);

/** \brief Receive UDP packet. */
int32_t osal_socket_udp_receive(osal_socket *socket,
                                void *data,
                                size_t length,
                                uint16_t *port);

#endif
