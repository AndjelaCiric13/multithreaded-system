//
// Created by os on 5/15/23.
//

#include "../h/Scheduler.h"
//Lista<PCB*> Scheduler::readyThreads;

Scheduler::schedulerThread *Scheduler::head = nullptr;
Scheduler::schedulerThread *Scheduler::tail = nullptr;
int Scheduler::NUMS=0;
PCB *Scheduler::get() {
    schedulerThread*curr=head;
    while (Scheduler::SchedulerInstance().getCnt()!=0) {

        PCB *newPCB = curr->thread;
        if(newPCB->isFinished()) {
            head=head->next;
            MemoryAllocator::instanceMemAlloc().mem_free(curr);
            curr=head;
           // if(curr==tail) head=tail;
            if(curr== nullptr)head=tail=nullptr;
            continue;
        }
        newPCB->state = PCB::RUNNING;
        head=head->next;
        MemoryAllocator::instanceMemAlloc().mem_free(curr);
        curr=head;
        //if(curr==tail) head=tail;
        if(curr== nullptr)head=tail=nullptr;
        NUMS--;
        return newPCB;
    }
    return PCB::idleThread;
}

void Scheduler::put(PCB *t) {
    //schedulerThread* prev = nullptr;
    //if (tail)prev = tail;
    t->state=PCB::READY;
    tail = (!head ? head : tail->next) = (schedulerThread*)MemoryAllocator::instanceMemAlloc().mem_alloc(sizeof(schedulerThread)/MEM_BLOCK_SIZE +((sizeof (schedulerThread)%MEM_BLOCK_SIZE>0)?1:0));
    tail->thread= t;
    tail->next=nullptr;
    NUMS++;
    if(head== nullptr) head = tail;
}

int Scheduler::getCnt()const{
    int num=0;
    schedulerThread*curr = head;
    while(curr){
        num++;
        curr=curr->next;
    }
    return num;
}

int Scheduler::getBrojKorisnickih() const{
    int user =0;
    schedulerThread* curr = head;
    while(curr){
        if(curr->thread->kernelThread==0)user++;
        curr=curr->next;
    }
    return user;
}
