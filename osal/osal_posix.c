/**
 * @file osal_posix.c
 * @brief POSIX implementation of the operating system abstraction layer.
 */

#define _POSIX_C_SOURCE 200809L

#include "osal.h"

#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct osal_mutex
{
    pthread_mutex_t native;
};

struct osal_event
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool signaled;
};

struct osal_thread
{
    pthread_t native;
};

struct osal_process
{
    pid_t pid;
};

bool osal_init(void)
{
    return true;
}

void osal_shutdown(void)
{
}

typedef struct
{
    osal_thread_fn fn;
    void *arg;
} osal_thread_start;

static void *thread_trampoline(void *arg)
{
    osal_thread_start start = *(osal_thread_start *)arg;
    free(arg);
    start.fn(start.arg);
    return NULL;
}

osal_thread *osal_thread_create(osal_thread_fn fn, void *arg, int priority)
{
    (void)priority;
    osal_thread *thread = (osal_thread *)malloc(sizeof(osal_thread));
    if (thread == NULL)
    {
        return NULL;
    }
    osal_thread_start *wrapper = (osal_thread_start *)malloc(sizeof(osal_thread_start));
    if (wrapper == NULL)
    {
        free(thread);
        return NULL;
    }
    wrapper->fn = fn;
    wrapper->arg = arg;
    if (pthread_create(&thread->native, NULL, thread_trampoline, wrapper) != 0)
    {
        free(wrapper);
        free(thread);
        return NULL;
    }
    return thread;
}

void osal_thread_join(osal_thread *thread)
{
    if (thread != NULL)
    {
        (void)pthread_join(thread->native, NULL);
    }
}

void osal_thread_destroy(osal_thread *thread)
{
    if (thread != NULL)
    {
        free(thread);
    }
}

static void configure_realtime_policy(osal_process_class cls)
{
    if (cls == OSAL_PROCESS_CLASS_REALTIME)
    {
        struct sched_param sp;
        sp.sched_priority = 80;
        (void)sched_setscheduler(0, SCHED_FIFO, &sp);
    }
}

osal_process *osal_process_spawn(osal_process_fn fn, void *arg, osal_process_class cls)
{
    osal_process *proc = (osal_process *)malloc(sizeof(osal_process));
    if (proc == NULL)
    {
        return NULL;
    }
    pid_t pid = fork();
    if (pid < 0)
    {
        free(proc);
        return NULL;
    }
    if (pid == 0)
    {
        configure_realtime_policy(cls);
        fn(arg);
        _exit(EXIT_SUCCESS);
    }
    proc->pid = pid;
    return proc;
}

void osal_process_wait(osal_process *process)
{
    if (process == NULL)
    {
        return;
    }
    (void)waitpid(process->pid, NULL, 0);
}

void osal_process_destroy(osal_process *process)
{
    if (process != NULL)
    {
        free(process);
    }
}

osal_mutex *osal_mutex_create(void)
{
    osal_mutex *mutex = (osal_mutex *)malloc(sizeof(osal_mutex));
    if (mutex == NULL)
    {
        return NULL;
    }
    (void)pthread_mutex_init(&mutex->native, NULL);
    return mutex;
}

void osal_mutex_lock(osal_mutex *mutex)
{
    (void)pthread_mutex_lock(&mutex->native);
}

void osal_mutex_unlock(osal_mutex *mutex)
{
    (void)pthread_mutex_unlock(&mutex->native);
}

void osal_mutex_destroy(osal_mutex *mutex)
{
    if (mutex != NULL)
    {
        (void)pthread_mutex_destroy(&mutex->native);
        free(mutex);
    }
}

osal_event *osal_event_create(void)
{
    osal_event *event = (osal_event *)malloc(sizeof(osal_event));
    if (event == NULL)
    {
        return NULL;
    }
    (void)pthread_mutex_init(&event->mutex, NULL);
    (void)pthread_cond_init(&event->cond, NULL);
    event->signaled = false;
    return event;
}

bool osal_event_wait(osal_event *event, uint32_t timeout_ms)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000U;
    ts.tv_nsec += (long)(timeout_ms % 1000U) * 1000000L;
    if (ts.tv_nsec >= 1000000000L)
    {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000L;
    }
    (void)pthread_mutex_lock(&event->mutex);
    while (!event->signaled)
    {
        if (pthread_cond_timedwait(&event->cond, &event->mutex, &ts) == ETIMEDOUT)
        {
            (void)pthread_mutex_unlock(&event->mutex);
            return false;
        }
    }
    event->signaled = false;
    (void)pthread_mutex_unlock(&event->mutex);
    return true;
}

void osal_event_signal(osal_event *event)
{
    (void)pthread_mutex_lock(&event->mutex);
    event->signaled = true;
    (void)pthread_cond_signal(&event->cond);
    (void)pthread_mutex_unlock(&event->mutex);
}

void osal_event_destroy(osal_event *event)
{
    if (event != NULL)
    {
        (void)pthread_mutex_destroy(&event->mutex);
        (void)pthread_cond_destroy(&event->cond);
        free(event);
    }
}

void osal_sleep_ms(uint32_t ms)
{
    struct timespec req;
    req.tv_sec = ms / 1000U;
    req.tv_nsec = (long)(ms % 1000U) * 1000000L;
    nanosleep(&req, NULL);
}

uint64_t osal_time_us(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000ULL + (uint64_t)ts.tv_nsec / 1000ULL;
}
