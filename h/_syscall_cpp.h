//
// Created by os on 6/21/23.
//

#ifndef PROJEKAT__SYSCALL_CPP_H
#define PROJEKAT__SYSCALL_CPP_H

#include "../h/syscall_c.h"
//#include "../lib/console.h"
//#include "../lib/hw.h"
//#include "../h/memory_allocator.h"
#include "../h/PCB.h"
#include "../h/Sem.h"

void* operator new (size_t);
void operator delete(void*);


class Thread{
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread();
    int start ();
    void join();
    static void dispatch ();
    static int sleep (time_t);
    static void runWrapper(void*);
protected:
    static void finish(void*);
    Thread ();
    virtual void run () {}
private:
    thread_t myHandle;
    void (*body)(void*); void* arg;
};
class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
private:
    sem_t myHandle;
};
class PeriodicThread : public Thread {
public:
    void terminate () {
        Thread::finish(this);
    }
protected:
    PeriodicThread (time_t period);
    virtual void periodicActivation () {}
    void run()override{
        while(true){
            periodicActivation();
            time_sleep(period);
        }
    }
private:
    //static bool finished;
    time_t period;
};

class Console {
public:
    static char getc (){return ::getc();}
    static void putc (char c){::putc(c);}
};


#endif //PROJEKAT__SYSCALL_CPP_H
