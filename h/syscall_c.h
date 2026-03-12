#ifndef SYSCALL_C_H
#define SYSCALL_C_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "../lib/hw.h"

#ifdef __cplusplus
class TCB;
#else
typedef struct TCB TCB;
#endif // __cplusplus
typedef TCB _thread;

/*
#ifdef __cplusplus
class Sem;
#else
typedef struct Sem Sem;
#endif // __cplusplus
typedef Sem _sem;
*/

void *mem_alloc(size_t size); // size in bytes

int mem_free(void *);

size_t mem_get_free_space();

size_t mem_get_largest_free_block();

typedef TCB _thread; // struct?
typedef _thread *thread_t;

int thread_create(
    thread_t *handle,
    void (*start_routine)(void *),
    void *arg
);

int thread_exit();

void thread_dispatch();

/*
typedef struct Sem _sem; // without struct?
typedef _sem *sem_t;

int sem_open(
    sem_t *handle,
    unsigned init
);

int sem_close(sem_t handle);

int sem_wait(sem_t id);

int sem_signal(sem_t id);

typedef unsigned long time_t;

int time_sleep(time_t);

const int EOF = -1;

char getc();

void putc(char);
*/

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SYSCALL_C_H
