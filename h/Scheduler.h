//
// Created by os on 5/15/23.
//

#ifndef PROJEKAT_SCHEDULER_H
#define PROJEKAT_SCHEDULER_H
#include "../h/PCB.h"
#include "../h/lista.h"
#include "../lib/console.h"
class Scheduler {
private:

    struct schedulerThread{
        PCB* thread;
        schedulerThread*next;
        schedulerThread(PCB*thread, size_t sleepingTime):thread(thread), next(nullptr){}
    };

    static schedulerThread* head, *tail;

public:
    static Scheduler& SchedulerInstance(){
        static Scheduler instance;
        return instance;
    }
    int getCnt()const;
    int getBrojKorisnickih()const;
    PCB* get();
    void put(PCB *t);

    static int NUMS;

private:
    friend class PCB;
    friend class Sem;
   // static Lista<PCB*> readyThreads;
   // static Lista<PCB*> sleepingThreads;
    Scheduler(){};
    //~Scheduler(){};
};


#endif //PROJEKAT_SCHEDULER_H
