//
// Created by os on 6/22/23.
//

#ifndef PROJEKAT_SEM_H
#define PROJEKAT_SEM_H
#include "../h/lista.h"
#include "../h/PCB.h"
#include "../h/_syscall_cpp.h"
#include "../h/Scheduler.h"
#include "../h/memory_allocator.h"

class MemoryAllocator;


class Sem {
public:
    Sem(unsigned val);
   static int semOpen(Sem** sem, unsigned val);
   static int semClose(Sem* sem);

    static int semWait(Sem* sem);
    static int semSignal(Sem* sem);

    int getValue(){return value;}

    void* operator new(size_t size) {
        return (Sem*) MemoryAllocator::instanceMemAlloc().mem_alloc(size/ MEM_BLOCK_SIZE + (
                (size% MEM_BLOCK_SIZE>0)?1:0
                ));
    }

    void operator delete(void *p) {
        (MemoryAllocator::instanceMemAlloc()).mem_free(p);
    }
    static int NUMS;

protected:
    void block();
    void deblock();
private:

    struct semT{
        PCB* thread;
        semT*next;
        semT(PCB*thread, size_t sleepingTime):thread(thread), next(nullptr){}
    };

    semT* head = nullptr, *tail = nullptr;
    int value;
    int opened=0, closed=0;
    //Lista<PCB*> queueBlocked;


};


#endif //PROJEKAT_SEM_H
