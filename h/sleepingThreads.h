//
// Created by os on 6/22/23.
//

#ifndef PROJEKAT_SLEEPINGTHREADS_H
#define PROJEKAT_SLEEPINGTHREADS_H

#include "../h/PCB.h"
#include "../h/Scheduler.h"
#include "../h/_syscall_cpp.h"
#include "../h/lista.h"

class sleepingThreads {

private:
    friend class PCB;
public:
    struct sleepingThread{
        PCB* thread;
        size_t sleepingTime;
        sleepingThread*next;
        sleepingThread(PCB*thread, size_t sleepingTime):thread(thread), sleepingTime(sleepingTime), next(nullptr){}
    };

    static sleepingThread* head, *tail;

public:
    static sleepingThreads& sleepingQueue(){
        static sleepingThreads instance;
        return instance;
    }
    void operator=(sleepingThreads&& s)=delete;
    void operator=(const sleepingThreads& s)=delete;
    sleepingThreads(sleepingThreads&& s ) = delete;
    sleepingThreads(const sleepingThreads& s) = delete;
    //~sleepingThreads(){};
    void addThreadToSleep(PCB* thread, size_t timeToSleep);
    void subOneTimeSlice();
    void checkToRemoveThread();
private:
    //static Lista<sleepingThread*> sleepingThreadQueue;
    sleepingThreads(){}
};


#endif //PROJEKAT_SLEEPINGTHREADS_H
