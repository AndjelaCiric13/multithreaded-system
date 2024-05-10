//
// Created by os on 6/22/23.
//

#include "../h/Sem.h"
#include "../h/workers.h"

int Sem::NUMS=0;

void Sem::block() {
    PCB::currentRunning->state = PCB::BLOCKED;
    tail = (!head ? head : tail->next) = (semT*)MemoryAllocator::instanceMemAlloc().mem_alloc(sizeof(semT)/MEM_BLOCK_SIZE +((sizeof (semT)%MEM_BLOCK_SIZE>0)?1:0));
    tail->thread= PCB::currentRunning; tail->next=nullptr;
    if(head== nullptr) head = tail;
    NUMS++;
    //thread_dispatch();

    PCB* old = PCB::currentRunning;
    PCB* newThread= nullptr;
    if(Scheduler::SchedulerInstance().getCnt()==0) {
        //uint64*idleStack = (uint64*)MemoryAllocator::instanceMemAlloc().mem_alloc((DEFAULT_STACK_SIZE*sizeof(uint64))/MEM_BLOCK_SIZE
        //                                                                         +((DEFAULT_STACK_SIZE*sizeof(uint64)%MEM_BLOCK_SIZE > 0)? 1:0) );
        //PCB*idle = new PCB(addIdleThread, nullptr, idleStack);
//        Scheduler::SchedulerInstance().put(PCB::idleThread);
        newThread=PCB::idleThread;
    }
    else newThread = Scheduler::SchedulerInstance().get();

    PCB::currentRunning = newThread;
    PCB::currentRunning->state = PCB::RUNNING;
    PCB::contextSwitch(&old->context, &PCB::currentRunning->context);
}

void Sem::deblock() {
    semT*curr = head;
    NUMS--;
    PCB*thread = curr->thread;
    head=head->next;
    if (head== nullptr) tail=nullptr;

    thread->state = PCB::READY;
    Scheduler::SchedulerInstance().put(thread);
    MemoryAllocator::instanceMemAlloc().mem_free(curr);

}

Sem::Sem(unsigned val){
    value = (int)val;
    opened=0; closed=0;
}

int Sem::semOpen( Sem** semA, unsigned val) {
    *semA = new Sem(val);
    if((*semA)==nullptr) return -1;
    (*semA)->opened=1;
    return 0;
}

int Sem::semClose(Sem* sem) {
    if(sem->closed) return -1;
    semT*curr = sem->head;
    while(curr){
        PCB*cur =curr->thread;
        cur->setAfterSemClosing();
        sem->head = sem->head->next;
        cur->state = PCB::READY;
        Scheduler::SchedulerInstance().put(cur);
        MemoryAllocator::instanceMemAlloc().mem_free(curr);
        curr =sem->head;
    }
    sem->head = sem->tail= nullptr;
    sem->closed =1;
    return 0;
}

int Sem::semWait(Sem* sem) {
    (sem->value)--;
    if(sem->value<0){
        sem->block();
    }
    if(sem->closed){
        if(PCB::currentRunning->releasedAfterClosing() == 1) {
            PCB::currentRunning->resetAfterSemClosing();return -1;}
        else return 0;
    }
    return 0;
}

int Sem::semSignal(Sem* sem) {
    if((++(sem->value))<=0){
        sem->deblock();
        return 0;
    }
    else return -1;
}