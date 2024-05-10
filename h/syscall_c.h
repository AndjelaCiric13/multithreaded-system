#ifndef syscall_c
#define syscall_c

#include "../lib/hw.h"
static const uint8 CODE_MEM_ALLOC=0x01;
static const uint8 CODE_MEM_FREE=0x02;
static const uint8 CODE_THREAD_CREATE = 0x11;
static const uint8 CODE_THREAD_EXIT = 0x12;
static const uint8 CODE_THREAD_DISPATCH = 0x13;
static const uint8 CODE_THREAD_JOIN = 0x14;
static const uint8 CODE_SEM_OPEN= 0x21;
static const uint8 CODE_SEM_CLOSE = 0x22;
static const uint8 CODE_SEM_WAIT = 0x23;
static const uint8 CODE_SEM_SIGNAL=0x24;
static const uint8 CODE_THREAD_SLEEP = 0x31;
static const uint8 CODE_GETC = 0x41;
static const uint8 CODE_PUTC = 0x42;
static const int buff_size = 1024;
void *mem_alloc (size_t size);
int mem_free(void*);


class PCB;
typedef PCB* thread_t;
int thread_create (
    thread_t* handle,
    void(*start_routine) (void*),
    void *arg
);

int thread_exit();

void thread_dispatch();

void thread_join(
        thread_t handle
        );

class Sem;
typedef Sem* sem_t;
int sem_open (
        sem_t* handle,
        unsigned init
        );

int sem_close(sem_t handle);

int sem_wait (sem_t id);

int sem_signal(sem_t id);

typedef unsigned long time_t;
int time_sleep(time_t);

const int EOF=-1;
char getc();
void putc(char);

#endif