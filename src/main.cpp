//
// Created by os on 5/12/23.
//

#include "../lib/console.h"
#include "../h/printing.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"
#include "../h/Scheduler.h"
#include "../h/Sem.h"
#include "../h/workers.h"
#include "../h/PCB.h"
#include "../h/MyBuffer.h"


uint64* kernelStack asm ("ssp");

Sem*sem;
bool mainFinished = false;
extern void userMain();

void userMainWrapper(void*){
    printString("UserMain started\n");
    userMain();
    printString("UserMain finished\n");
}


void  outputConsoleThread(void*){
    while(true){
       while((*((char*)CONSOLE_STATUS) & CONSOLE_TX_STATUS_BIT )){
            char c = MyBuffer::outputBuffer->getcB();
           //asm volatile("csrr t6, sepc");

            *((char*)CONSOLE_TX_DATA)=c;
        }
       thread_dispatch();
    }
}

class Periodic:public PeriodicThread{
public:
    Periodic(time_t t): PeriodicThread(t){}

protected:
    virtual void periodicActivation() override{
        printString("Periodic\n");
    }
};

int main(){
    kernelStack=(uint64*)(HEAP_END_ADDR)-1;
    Riscv::w_stvec((uint64) (&Riscv::supervisorTrap) |1);

    PCB* thread1;
    thread_create(&thread1, nullptr,nullptr);



    PCB::currentRunning=thread1;
    MyBuffer::outputBuffer = new MyBuffer();
    MyBuffer::inputBuffer = new MyBuffer();

    uint64*idleStack = (uint64*)MemoryAllocator::instanceMemAlloc().mem_alloc((DEFAULT_STACK_SIZE*8)/MEM_BLOCK_SIZE
                                                                              +((DEFAULT_STACK_SIZE*8%MEM_BLOCK_SIZE > 0)? 1:0) );
    PCB::idleThread= new PCB(addIdleThread, nullptr, idleStack);

    thread_create(&PCB::outputThread, outputConsoleThread, nullptr);
    PCB::outputThread->setKernelThread();
    PCB* thread2;
    thread_create(&thread2, userMainWrapper,nullptr);
   

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);


    thread_join(thread2);



    printString("MainFinished\n");
    while((MyBuffer::outputBuffer->getCnt() !=0)){
    }
   delete thread2;
   delete PCB::outputThread;

    return 0;
}


