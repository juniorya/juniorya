#define _POSIX_C_SOURCE 200809L

#include "osal/osal.h"

#include "utils/log.h"

#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
/* Stubs for Baget - replace with RTOS primitives when integrating. */
#include <stdlib.h>
#include <string.h>
#include <time.h>
#endif

typedef struct timer_wrapper
{
    osal_timer_cb cb;
    void *arg;
    uint32_t period_us;
    bool running;
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    pthread_t thread;
#endif
} timer_wrapper;

typedef struct event_wrapper
{
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    pthread_cond_t *cond;
    pthread_mutex_t *mutex;
#endif
    bool signaled;
} event_wrapper;

typedef struct
{
    osal_thread_entry entry;
    void *arg;
} thread_bootstrap;

static void *timer_thread(void *arg)
{
    timer_wrapper *timer = (timer_wrapper *)arg;
    struct timespec sleep_ts;
    sleep_ts.tv_sec = 0;
    sleep_ts.tv_nsec = (long)timer->period_us * 1000L;
    while (timer->running)
    {
        nanosleep(&sleep_ts, NULL);
        timer->cb(timer->arg);
    }
    return NULL;
}

static void *osal_thread_trampoline(void *ctx)
{
    thread_bootstrap *bootstrap = (thread_bootstrap *)ctx;
    bootstrap->entry(bootstrap->arg);
    free(bootstrap);
    return NULL;
}

bool osal_init(void)
{
    log_init();
    return true;
}

void osal_shutdown(void)
{
}

bool osal_thread_create(osal_thread *thread,
                        osal_thread_entry entry,
                        void *arg,
                        osal_process_class cls,
                        const char *name)
{
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    (void)name;
    pthread_t *handle = malloc(sizeof(pthread_t));
    if (handle == NULL)
    {
        return false;
    }
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if (cls == OSAL_PROCESS_CLASS_REALTIME)
    {
        struct sched_param param;
        param.sched_priority = 80;
        pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        pthread_attr_setschedparam(&attr, &param);
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    }
    thread_bootstrap *bootstrap = malloc(sizeof(thread_bootstrap));
    if (bootstrap == NULL)
    {
        free(handle);
        pthread_attr_destroy(&attr);
        return false;
    }
    bootstrap->entry = entry;
    bootstrap->arg = arg;
    int rc = pthread_create(handle, &attr, osal_thread_trampoline, bootstrap);
    pthread_attr_destroy(&attr);
    if (rc != 0)
    {
        free(handle);
        return false;
    }
    thread->handle = handle;
    return true;
#else
    (void)thread;
    (void)entry;
    (void)arg;
    (void)cls;
    (void)name;
    return false;
#endif
}

void osal_thread_join(osal_thread *thread)
{
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    if ((thread != NULL) && (thread->handle != NULL))
    {
        pthread_join(*(pthread_t *)thread->handle, NULL);
    }
#else
    (void)thread;
#endif
}

void osal_thread_destroy(osal_thread *thread)
{
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    if ((thread != NULL) && (thread->handle != NULL))
    {
        free(thread->handle);
        thread->handle = NULL;
    }
#else
    (void)thread;
#endif
}

bool osal_mutex_init(osal_mutex *mutex)
{
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    pthread_mutex_t *handle = malloc(sizeof(pthread_mutex_t));
    if (handle == NULL)
    {
        return false;
    }
    pthread_mutex_init(handle, NULL);
    mutex->handle = handle;
    return true;
#else
    (void)mutex;
    return false;
#endif
}

void osal_mutex_lock(osal_mutex *mutex)
{
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    pthread_mutex_lock((pthread_mutex_t *)mutex->handle);
#else
    (void)mutex;
#endif
}

void osal_mutex_unlock(osal_mutex *mutex)
{
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    pthread_mutex_unlock((pthread_mutex_t *)mutex->handle);
#else
    (void)mutex;
#endif
}

void osal_mutex_destroy(osal_mutex *mutex)
{
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    if ((mutex != NULL) && (mutex->handle != NULL))
    {
        pthread_mutex_destroy((pthread_mutex_t *)mutex->handle);
        free(mutex->handle);
        mutex->handle = NULL;
    }
#else
    (void)mutex;
#endif
}

bool osal_event_init(osal_event *event)
{
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    pthread_cond_t *cond = malloc(sizeof(pthread_cond_t));
    pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
    if ((cond == NULL) || (mutex == NULL))
    {
        free(cond);
        free(mutex);
        return false;
    }
    pthread_cond_init(cond, NULL);
    pthread_mutex_init(mutex, NULL);
    event_wrapper *wrapper = malloc(sizeof(*wrapper));
    if (wrapper == NULL)
    {
        free(cond);
        free(mutex);
        return false;
    }
    wrapper->cond = cond;
    wrapper->mutex = mutex;
    wrapper->signaled = false;
    event->handle = wrapper;
    return true;
#else
    (void)event;
    return false;
#endif
}

void osal_event_signal(osal_event *event)
{
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    if ((event == NULL) || (event->handle == NULL))
    {
        return;
    }
    event_wrapper *wrapper = event->handle;
    pthread_mutex_lock(wrapper->mutex);
    wrapper->signaled = true;
    pthread_cond_signal(wrapper->cond);
    pthread_mutex_unlock(wrapper->mutex);
#else
    (void)event;
#endif
}

bool osal_event_wait(osal_event *event, uint32_t timeout_ms)
{
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    if ((event == NULL) || (event->handle == NULL))
    {
        return false;
    }
    event_wrapper *wrapper = event->handle;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000U;
    ts.tv_nsec += (timeout_ms % 1000U) * 1000000L;
    if (ts.tv_nsec >= 1000000000L)
    {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000L;
    }
    pthread_mutex_lock(wrapper->mutex);
    while (!wrapper->signaled)
    {
        if (pthread_cond_timedwait(wrapper->cond, wrapper->mutex, &ts) == ETIMEDOUT)
        {
            pthread_mutex_unlock(wrapper->mutex);
            return false;
        }
    }
    wrapper->signaled = false;
    pthread_mutex_unlock(wrapper->mutex);
    return true;
#else
    (void)event;
    (void)timeout_ms;
    return false;
#endif
}

void osal_event_destroy(osal_event *event)
{
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    if ((event == NULL) || (event->handle == NULL))
    {
        return;
    }
    event_wrapper *wrapper = event->handle;
    pthread_cond_destroy(wrapper->cond);
    pthread_mutex_destroy(wrapper->mutex);
    free(wrapper->cond);
    free(wrapper->mutex);
    free(wrapper);
    event->handle = NULL;
#else
    (void)event;
#endif
}

bool osal_timer_start(osal_timer *timer,
                      uint32_t period_us,
                      osal_timer_cb cb,
                      void *arg)
{
    if ((timer == NULL) || (cb == NULL) || (period_us == 0U))
    {
        return false;
    }
    timer_wrapper *wrapper = malloc(sizeof(timer_wrapper));
    if (wrapper == NULL)
    {
        return false;
    }
    wrapper->cb = cb;
    wrapper->arg = arg;
    wrapper->period_us = period_us;
    wrapper->running = true;
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    pthread_create(&wrapper->thread, NULL, timer_thread, wrapper);
#endif
    timer->handle = wrapper;
    return true;
}

void osal_timer_stop(osal_timer *timer)
{
    if ((timer == NULL) || (timer->handle == NULL))
    {
        return;
    }
    timer_wrapper *wrapper = timer->handle;
    wrapper->running = false;
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    pthread_join(wrapper->thread, NULL);
#endif
    free(wrapper);
    timer->handle = NULL;
}

void osal_sleep_ms(uint32_t ms)
{
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    struct timespec ts;
    ts.tv_sec = ms / 1000U;
    ts.tv_nsec = (long)(ms % 1000U) * 1000000L;
    nanosleep(&ts, NULL);
#else
    (void)ms;
#endif
}

uint64_t osal_clock_monotonic_ns(void)
{
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ((uint64_t)ts.tv_sec * 1000000000ULL) + (uint64_t)ts.tv_nsec;
#else
    return (uint64_t)time(NULL) * 1000000000ULL;
#endif
}

bool osal_affinity_set(osal_process_class cls, uint32_t core_index)
{
    (void)cls;
    (void)core_index;
    return false;
}

bool osal_socket_udp_open(osal_socket *sock, uint16_t port)
{
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    int fd = (int)socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        return false;
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        close(fd);
        return false;
    }
    sock->fd = fd;
    return true;
#else
    (void)sock;
    (void)port;
    return false;
#endif
}

void osal_socket_close(osal_socket *sock)
{
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    if ((sock != NULL) && (sock->fd >= 0))
    {
        close(sock->fd);
        sock->fd = -1;
    }
#else
    (void)sock;
#endif
}

int32_t osal_socket_udp_send(osal_socket *sock,
                             const void *data,
                             size_t length,
                             const char *address,
                             uint16_t port)
{
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address);
    return (int32_t)sendto(sock->fd, data, length, 0, (struct sockaddr *)&addr, sizeof(addr));
#else
    (void)sock;
    (void)data;
    (void)length;
    (void)address;
    (void)port;
    return -1;
#endif
}

int32_t osal_socket_udp_receive(osal_socket *sock,
                                void *data,
                                size_t length,
                                uint16_t *port)
{
#if defined(TARGET_OS_PC_LINUX) || defined(TARGET_OS_QEMU_MIPS64)
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int32_t ret = (int32_t)recvfrom(sock->fd, data, length, 0, (struct sockaddr *)&addr, &addrlen);
    if ((ret >= 0) && (port != NULL))
    {
        *port = ntohs(addr.sin_port);
    }
    return ret;
#else
    (void)sock;
    (void)data;
    (void)length;
    (void)port;
    return -1;
#endif
}
