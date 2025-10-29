/**
 * @file osal_host.c
 * @brief POSIX host implementation of the OS abstraction layer.
 */
#include "osal.h"

#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

struct osal_thread_impl
{
    pthread_t thread;
};

struct osal_mutex_impl
{
    pthread_mutex_t mutex;
};

struct osal_event_impl
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool signaled;
};

bool osal_init(void)
{
    return true;
}

struct thread_wrapper
{
    osal_thread_fn fn;
    void *arg;
};

static void *osal_thread_trampoline(void *arg)
{
    struct thread_wrapper *wrapper = (struct thread_wrapper *)arg;
    wrapper->fn(wrapper->arg);
    free(wrapper);
    return NULL;
}

bool osal_thread_create(osal_thread_t *thread,
                        osal_thread_fn fn,
                        void *arg,
                        int priority)
{
    (void)priority;
    thread->impl = malloc(sizeof(struct osal_thread_impl));
    if (thread->impl == NULL)
    {
        return false;
    }
    struct osal_thread_impl *impl = (struct osal_thread_impl *)thread->impl;
    struct thread_wrapper *wrapper = malloc(sizeof(*wrapper));
    if (wrapper == NULL)
    {
        free(thread->impl);
        thread->impl = NULL;
        return false;
    }
    wrapper->fn = fn;
    wrapper->arg = arg;
    if (pthread_create(&impl->thread, NULL, osal_thread_trampoline, wrapper) != 0)
    {
        free(wrapper);
        free(thread->impl);
        thread->impl = NULL;
        return false;
    }
    return true;
}

void osal_thread_join(osal_thread_t *thread)
{
    if (thread->impl == NULL)
    {
        return;
    }
    struct osal_thread_impl *impl = (struct osal_thread_impl *)thread->impl;
    (void)pthread_join(impl->thread, NULL);
    free(thread->impl);
    thread->impl = NULL;
}

bool osal_mutex_init(osal_mutex_t *mutex)
{
    mutex->impl = malloc(sizeof(struct osal_mutex_impl));
    if (mutex->impl == NULL)
    {
        return false;
    }
    struct osal_mutex_impl *impl = (struct osal_mutex_impl *)mutex->impl;
    return pthread_mutex_init(&impl->mutex, NULL) == 0;
}

void osal_mutex_lock(osal_mutex_t *mutex)
{
    struct osal_mutex_impl *impl = (struct osal_mutex_impl *)mutex->impl;
    (void)pthread_mutex_lock(&impl->mutex);
}

void osal_mutex_unlock(osal_mutex_t *mutex)
{
    struct osal_mutex_impl *impl = (struct osal_mutex_impl *)mutex->impl;
    (void)pthread_mutex_unlock(&impl->mutex);
}

bool osal_event_init(osal_event_t *event)
{
    event->impl = malloc(sizeof(struct osal_event_impl));
    if (event->impl == NULL)
    {
        return false;
    }
    struct osal_event_impl *impl = (struct osal_event_impl *)event->impl;
    impl->signaled = false;
    (void)pthread_mutex_init(&impl->mutex, NULL);
    (void)pthread_cond_init(&impl->cond, NULL);
    return true;
}

void osal_event_signal(osal_event_t *event)
{
    struct osal_event_impl *impl = (struct osal_event_impl *)event->impl;
    (void)pthread_mutex_lock(&impl->mutex);
    impl->signaled = true;
    (void)pthread_cond_signal(&impl->cond);
    (void)pthread_mutex_unlock(&impl->mutex);
}

bool osal_event_wait(osal_event_t *event, uint32_t timeout_ms)
{
    struct osal_event_impl *impl = (struct osal_event_impl *)event->impl;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000U;
    ts.tv_nsec += (timeout_ms % 1000U) * 1000000L;
    if (ts.tv_nsec >= 1000000000L)
    {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000L;
    }
    bool signaled = false;
    (void)pthread_mutex_lock(&impl->mutex);
    while (!impl->signaled)
    {
        if (pthread_cond_timedwait(&impl->cond, &impl->mutex, &ts) != 0)
        {
            break;
        }
    }
    if (impl->signaled)
    {
        signaled = true;
        impl->signaled = false;
    }
    (void)pthread_mutex_unlock(&impl->mutex);
    return signaled;
}

void osal_sleep_ms(uint32_t duration_ms)
{
    usleep(duration_ms * 1000U);
}
