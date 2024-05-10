//
// Created by os on 5/11/23.
//

#ifndef _syscall_cpp
#define _syscall_cpp

#include "../h/syscall_c.h"
#include "../h/_syscall_cpp.h"
//#include "../lib/console.h"
//#include "../lib/hw.h"
//#include "../h/memory_allocator.h"

void* operator new (size_t){   //bilo je void*::operator new pravi problem ::
    size_t size;
    __asm__ volatile ("mv %[size], a0" : [size] "=r"(size));
    return mem_alloc(size);
}
void operator delete(void*) {
    void* p;
    __asm__ volatile ("mv %[p], a0" : [p] "=r"(p));
    mem_free(p);
};

Thread::Thread(void (*body)(void *), void *arg) {
    myHandle = nullptr;
    this->body=body;
    this->arg=arg;
   // thread_create(&myHandle, body, arg);
    //thread_create(&myHandle, Thread::runWrapper, (void*)this);
   // myHandle->setCreatedFromThread();
}

Thread::~Thread(){
    //thread_exit();
    myHandle->setFinished();
    delete myHandle;
}

int Thread::start(){
   // int ret =myHandle->start();
    int ret =thread_create(&myHandle, body, arg);
    return ret;
}

void Thread::join() {
    thread_join(myHandle);
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t t) {
    int ret =time_sleep(t);
    return ret;
}


Thread::Thread(){
    myHandle =nullptr;
    //thread_create(&myHandle, Thread::runWrapper, (void*)this);
    this->body=Thread::runWrapper;
    this->arg=(void*)this;
}

void Thread::runWrapper(void * t) {
    //if(t==nullptr) return;
    ((Thread*)t)->run();
}

void Thread::finish(void * t) {
    ((Thread*)t)->myHandle->setFinished();
}

Semaphore::Semaphore(unsigned init) {
    myHandle=nullptr;
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {

        int ret = sem_wait(myHandle);
        return ret;
}

int Semaphore::signal() {
        int ret = sem_signal(myHandle);
        return ret;
}

PeriodicThread::PeriodicThread(time_t period) : Thread(){
    this->period = period;
}

//bool PeriodicThread::finished=false;

#endif

