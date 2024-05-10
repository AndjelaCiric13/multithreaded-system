//
// Created by os on 6/22/23.
//

#include "../h/sleepingThreads.h"
//Lista <sleepingThreads::sleepingThread* > sleepingThreads::sleepingThreadQueue;

sleepingThreads::sleepingThread* sleepingThreads:: head = nullptr;
sleepingThreads::sleepingThread* sleepingThreads:: tail = nullptr;

void sleepingThreads::addThreadToSleep(PCB *thread, size_t timeToSleep) {

    thread->state = PCB::SLEEPING;
    sleepingThread* newElem = (sleepingThread*)(MemoryAllocator::instanceMemAlloc().mem_alloc(sizeof(sleepingThread)/MEM_BLOCK_SIZE +
            ((sizeof(sleepingThread)%MEM_BLOCK_SIZE>0)? 1:0)));
         newElem->next=nullptr;
         newElem->sleepingTime=timeToSleep;
         newElem->thread=thread;
    if(head== nullptr) {
        head = newElem;
        tail = newElem;
    }
    else{
        tail =tail->next = newElem;
    }
}
void sleepingThreads::checkToRemoveThread() {

    sleepingThread* curr=head, *prev = nullptr;
    while(curr) {
        if (curr->sleepingTime == 0) {
            if (curr == head) {
                head = head->next;
                curr->thread->state = PCB::READY;
                Scheduler::SchedulerInstance().put(curr->thread);
                if (head == nullptr) tail = nullptr;
                MemoryAllocator::instanceMemAlloc().mem_free(curr);
                curr = head;
            } else {
                prev->next = curr->next;
                if (curr == tail) tail = prev;
                sleepingThread *deleteT = curr;
                Scheduler::SchedulerInstance().put(curr->thread);
                curr = curr->next;
                MemoryAllocator::instanceMemAlloc().mem_free(deleteT);
            }
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}
void sleepingThreads::subOneTimeSlice() {

    sleepingThread* curr = head;
    while(curr){
        curr->sleepingTime--;
        curr=curr->next;
    }
}


/*
void sleepingThreads::addThreadToSleep(PCB *thread, size_t timeToSleep) {
    thread->state = PCB::SLEEPING;
    sleepingThread* s = new sleepingThread(thread, timeToSleep);
    sleepingThreadQueue.dodajNaKraj(s);

}

void sleepingThreads::subOneTimeSlice() {

    sleepingThreadQueue.postaviNaPrvi();
    while(sleepingThreadQueue.imaTek()){
        sleepingThread*curr = sleepingThreadQueue.dohvatiCurrPod();
        curr->sleepingTime--;
        sleepingThreadQueue.naSled();
    }
}

void sleepingThreads::checkToRemoveThread() {

    sleepingThread* curr=head, *prev = nullptr;
    while(curr) {
        if (curr->sleepingTime == 0) {
            if (curr == head) {
                head = head->next;
                curr->thread->state = PCB::READY;
                Scheduler::SchedulerInstance().put(curr->thread);
                if (head == nullptr) tail = nullptr;
                delete curr;
                curr = head;
            } else {
                prev->next = curr->next;
                if (curr == tail) tail = prev;
                sleepingThread *deleteT = curr;
                curr = curr->next;
                delete deleteT;
            }
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}
*/