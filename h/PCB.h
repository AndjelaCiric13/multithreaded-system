//
// Created by os on 5/15/23.
//

#ifndef PROJEKAT_PCB_H
#define PROJEKAT_PCB_H

#include "../lib/hw.h"
#include "../h/syscall_c.h"
#include "../h/_syscall_cpp.h"
//#include "../h/sleepingThreads.h"
//#include "../h/Scheduler.h"
// #include "../h/Sem.h"
#include "../h/lista.h"
#include "../h/memory_allocator.h"

class MemoryAllocator;

class PCB{
public:

    static PCB* idleThread;
    static PCB*outputThread;
    static uint64*idleStack;
    char ind;
    static uint64 frameP asm("myFramePointer");
    PCB (void (*body)(void*), void* arg, uint64*s);
    virtual ~PCB ();
    int start ();
    void join();


    void setKernelThread(){kernelThread=1;}
    static void resetTimeSliceCounter(){timeSliceCounter=0;}

    using Body = void (*)(void *);
    uint64 getTimeSlice(){return timeSlice;}
    static void dispatch ();
    static int sleep (time_t time);
    static PCB* currentRunning asm("currentRunning");
    static void yield();

    struct Context{
        uint64 ra;
        uint64 sp;
    };

    void setCreatedFromThread();
    int isCreatedFromThread();
    static PCB *createThread(Body body, void*arg, char c);
    static void createThreadWithStack(PCB**,Body body, void *arg, uint64* stack);

    static void wrapper();

    void setFinished(){finishedThread=true; state = PCB::FINISHED;}

    bool isFinished() const {return finishedThread;};

    void* operator new(size_t size) {
        return (PCB*) MemoryAllocator::instanceMemAlloc().mem_alloc(size/ MEM_BLOCK_SIZE + (
                (size%MEM_BLOCK_SIZE>0)?1:0
        ));
    }

    void operator delete(void *p) {
        (MemoryAllocator::instanceMemAlloc()).mem_free(p);
    }
protected:
    PCB ();
    virtual void run () ;
private:

    struct joinThread{
        PCB* thread;
        joinThread*next;
        joinThread(PCB*thread, size_t sleepingTime):thread(thread), next(nullptr){}
    };

     joinThread* head= nullptr, *tail= nullptr;

    Context context;

    void (*body)(void*) = nullptr; void* arg;
    int kernelThread=0;
    bool finishedThread = false;

    int createdFromThread=0;

    static uint64 timeSliceCounter;

    friend class sleepingThreads;
    friend class Sem;
    enum Thread_state{ RUNNING, SLEEPING, READY, FINISHED, STARTED, CREATED, BLOCKED, JOINED};
    Thread_state state=STARTED;


    friend class Riscv;
    friend class Scheduler;
    static void contextSwitch(Context* oldT, Context* newT);

public:
    static uint64 constexpr stackSize = DEFAULT_STACK_SIZE;
private:
    static uint64 constexpr defaultTimeSlice = DEFAULT_TIME_SLICE;
    uint64* userStack;
    uint64 timeSlice;

    //Lista<PCB*> waitingOnJoin;


    static void setAllFinished();
public:
    int releasedAfterClosing();
    void setAfterSemClosing();
    void resetAfterSemClosing();
private:
    int isReleasedAfterClosing=0;
};


#endif //PROJEKAT_PCB_H
