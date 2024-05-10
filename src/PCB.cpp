//
// Created by os on 5/15/23.
//

#include "../h/PCB.h"
#include "../h/Scheduler.h"
#include "../h/riscv.hpp"
#include "../h/sleepingThreads.h"
#include "../h/Scheduler.h"
#include "../h/printing.hpp"
#include "../h/workers.h"
#include "../h/MyBuffer.h"



PCB* PCB::currentRunning=nullptr;
uint64 PCB::timeSliceCounter=0;
                                                                           
PCB* PCB::idleThread= nullptr;
PCB* PCB::outputThread= nullptr;

PCB::~PCB() {MemoryAllocator::instanceMemAlloc().mem_free(userStack) ;
}

PCB::PCB(void (*body)(void *), void *arg, uint64*stack) {
    this->body=body, this->arg=arg;
    userStack=stack;
                                                                                                            
    context = {
            (uint64) &wrapper,
            userStack!= nullptr ? (uint64) &userStack[stackSize] :0
    };
    timeSlice=defaultTimeSlice;
    state = CREATED;
    finishedThread=false;
    if(body== nullptr) kernelThread=1;
    else kernelThread=0;
}


int PCB::start() {
    if (currentRunning == nullptr )  {
        currentRunning = this;
    }
    Scheduler::SchedulerInstance().put(this);
    return 0;
}

void PCB::join() {
    if(this->finishedThread)
        return;
    tail = (!head ? head : tail->next) = (joinThread*)MemoryAllocator::instanceMemAlloc().mem_alloc(sizeof(joinThread)/MEM_BLOCK_SIZE +((sizeof (joinThread)%MEM_BLOCK_SIZE>0)?1:0));
    tail->thread= currentRunning; tail->next=nullptr;
    if(head== nullptr) head = tail;
    PCB* oldThread = currentRunning;
    currentRunning->state = PCB::JOINED;
    PCB* newThread= nullptr;
    if(Scheduler::SchedulerInstance().getCnt()==0) {
        newThread=idleThread;
    }

    else newThread = Scheduler::SchedulerInstance().get();

    currentRunning=newThread;
    currentRunning->state = PCB::RUNNING;
    PCB::contextSwitch(&oldThread->context, &currentRunning->context);
}


void PCB::dispatch() {
    PCB* oldThread = currentRunning;
    if(!(oldThread->isFinished() ) && !(currentRunning->state==PCB::BLOCKED)&& !(currentRunning->state==PCB::JOINED) &&
                                                                               !(currentRunning == idleThread))
                                                                               {
        Scheduler::SchedulerInstance().put(oldThread);
    }
    PCB* newThread= nullptr;
    if(Scheduler::SchedulerInstance().getCnt()==0) {
        
        newThread=idleThread;
    }

    else newThread = Scheduler::SchedulerInstance().get();

   
    currentRunning=newThread;
    currentRunning->state = PCB::RUNNING;
    PCB::contextSwitch(&oldThread->context, &currentRunning->context);
}

int PCB::sleep(time_t time) {
    if(currentRunning->finishedThread) return -1;
    currentRunning->state = PCB::SLEEPING;
    sleepingThreads::sleepingQueue().addThreadToSleep(PCB::currentRunning, time );
    PCB* newThread= nullptr;
    PCB*oldThread = currentRunning;
    if(Scheduler::SchedulerInstance().getCnt()==0) {
        
        newThread=idleThread;
    }
    else {newThread = Scheduler::SchedulerInstance().get();}

  
    currentRunning=newThread;
    currentRunning->state = PCB::RUNNING;
    PCB::contextSwitch(&oldThread->context, &currentRunning->context);
    return 0;
}

PCB::PCB() {
    userStack = (this->body!= nullptr) ? (uint64*)MemoryAllocator::instanceMemAlloc().mem_alloc((stackSize*sizeof(uint64))/MEM_BLOCK_SIZE
                                                                                                +((stackSize*sizeof(uint64)%MEM_BLOCK_SIZE > 0)? 1:0) ) : nullptr;
   
    context = {
            (uint64) &wrapper,
            userStack!= nullptr ? (uint64) &userStack[stackSize] : 0
    };
    timeSlice=defaultTimeSlice;
    state = CREATED;
    finishedThread=false;
}

void PCB::yield() {
    uint8 code=CODE_THREAD_DISPATCH;
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (code));
    __asm__ volatile("ecall");
}

PCB *PCB::createThread(Body body, void *arg, char i){

    PCB* newP = new PCB(body, arg, nullptr);
    newP->ind = i;
    return newP;
}

void PCB::createThreadWithStack(PCB** newP, Body body, void *arg, uint64* stack){
   
    *newP = new PCB(body, arg, stack);
    if((*newP)->body!= nullptr ) Scheduler::SchedulerInstance().put(*newP);
}

void PCB::wrapper() {
    Riscv::popSppSpie();
    currentRunning->run();
   thread_exit();
}

void PCB::run() {
    currentRunning->body(currentRunning->arg);
joinThread*curr = head;

    while(curr){
        PCB* tek = curr->thread;
        tek->state = PCB::READY;
        Scheduler::SchedulerInstance().put(tek);
        joinThread*old = curr;
        curr=curr->next;
        MemoryAllocator::instanceMemAlloc().mem_free(old);
    }
}

void PCB::setCreatedFromThread() {
    createdFromThread=1;
}

int PCB::isCreatedFromThread() {
    return createdFromThread;
}

void PCB::setAllFinished() {
    while(Scheduler::SchedulerInstance().getCnt()!=0){
        PCB*thread = Scheduler::SchedulerInstance().get();
        if(thread==outputThread)continue;
        else thread->setFinished();
    }
    Scheduler::SchedulerInstance().put(outputThread);
}

int PCB::releasedAfterClosing() {
    return this->isReleasedAfterClosing;
}

void PCB::resetAfterSemClosing(){
   this->isReleasedAfterClosing = 0;
}

void PCB::setAfterSemClosing() {
    this->isReleasedAfterClosing = 1;
}





