//
// Created by os on 5/13/23.
//

#include "../h/riscv.hpp"
#include "../h/syscall_c.h"
#include "../h/_syscall_cpp.h"
#include "../lib/console.h"
#include "../h/memory_allocator.h"
#include "../h/printing.hpp"
#include "../h/PCB.h"
#include "../h/sleepingThreads.h"
#include "../h/Sem.h"
#include "../h/MyBuffer.h"

extern PCB*outputThread;
int Riscv::mode=0;
void Riscv::popSppSpie()
{

    if(PCB::currentRunning->kernelThread==1){
        ms_sstatus(SSTATUS_SPP);
    }
    else {mc_sstatus(SSTATUS_SPP);}
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");

}


void Riscv::handleSupervisorTrap() {
    uint64 code, a1, a2, a3, a4;
    __asm__ volatile ("mv %[code], a0" : [code] "=r"(code));
    __asm__ volatile ("mv %[a1], a1" : [a1] "=r"(a1));
    __asm__ volatile ("mv %[a2], a2" : [a2] "=r"(a2));
    __asm__ volatile ("mv %[a3], a3" : [a3] "=r"(a3));
    __asm__ volatile ("mv %[a4], a4" : [a4] "=r"(a4));

    uint64 volatile scause=r_scause();
    uint64 volatile sepc = r_sepc();


    if(scause == 0x08 || scause==0x09){
        uint64 volatile sstatus = r_sstatus();

        sepc=sepc+4;

        switch(code){
            case CODE_MEM_ALLOC: {
                size_t size = a1;
                MemoryAllocator::instanceMemAlloc().mem_alloc(size);
                w_sstatus(sstatus);
                w_sepc(sepc);
                break;
            }
            case CODE_MEM_FREE: {
                void* p = (void*) a1;
                MemoryAllocator::instanceMemAlloc().mem_free(p);
                w_sstatus(sstatus);
                w_sepc(sepc);
                break;
            }
            case CODE_THREAD_CREATE:{
                thread_t * handle = (thread_t*) a1;
                PCB::Body body = (PCB::Body) a2;
                void*arg = (void*) a3;
                uint64*stack = (uint64*) a4;
                PCB::createThreadWithStack(handle, body, arg, stack);
                w_sstatus(sstatus);
                w_sepc(sepc);
                break;
            }
            case CODE_THREAD_EXIT:{
                PCB::currentRunning->setFinished();
                PCB*thread = PCB::currentRunning;
                PCB::resetTimeSliceCounter();
                PCB::dispatch();
                delete thread;
                thread = nullptr;
                w_sstatus(sstatus);
                w_sepc(sepc);
                break;
            }
            case CODE_THREAD_DISPATCH:{
                PCB::resetTimeSliceCounter();
                PCB::dispatch();
                w_sstatus(sstatus);
                w_sepc(sepc);
                break;
            }
            case CODE_THREAD_JOIN: {
                thread_t joinOn = (thread_t) a1;
                joinOn->join();
                w_sstatus(sstatus);
                w_sepc(sepc);
                break;
            }
            case CODE_SEM_OPEN: {
                sem_t* mySem = (sem_t*) a1;
                unsigned init = (unsigned) a2;
                Sem::semOpen(mySem, init);
                w_sstatus(sstatus);
                w_sepc(sepc);
                break;
            }
            case CODE_SEM_CLOSE: {
                Sem* handle = (Sem*) a1;
                Sem::semClose(handle);
                w_sstatus(sstatus);
                w_sepc(sepc);
                break;
            }
            case CODE_SEM_WAIT:{
                Sem* handle = (Sem*) a1;
                Sem::semWait(handle);
                w_sstatus(sstatus);
                w_sepc(sepc);
                break;
            }
            case CODE_SEM_SIGNAL:{
                Sem* handle = (Sem*) a1;
                Sem::semSignal(handle);
                w_sstatus(sstatus);
                w_sepc(sepc);
                break;
            }
            case CODE_THREAD_SLEEP: {
                time_t  time = (time_t) a1;
                PCB::sleep(time);
                w_sstatus(sstatus);
                w_sepc(sepc);
                break;
            }
            case CODE_GETC:{
                MyBuffer::inputBuffer->getcB();
                //__getc();
                w_sstatus(sstatus);
                w_sepc(sepc);
                break;
            }
            case CODE_PUTC:{
                char c = (char) a1;
                MyBuffer::outputBuffer->putcB(c);
                //__putc(c);
                w_sstatus(sstatus);
                w_sepc(sepc);
                break;
            }
            default: {
                break;
            }
        }

    }
    else{
        //uint64 volatile scause=r_scause();
        uint64 volatile sepc = r_sepc();
        uint64 volatile scause=r_scause();
        printString("scause: ");
        printInt(scause);
        printString("\n");
        printString("sepc: ");
        printInt(sepc);
        printString("\n");
        PCB::setAllFinished();
        PCB::currentRunning->setFinished();
        PCB::resetTimeSliceCounter();
        PCB::dispatch();
    }
}

void Riscv::timerTrapHandler() {
    uint64 volatile scause=r_scause();
    if(scause == 0x8000000000000001UL){  //timer

        PCB::timeSliceCounter++;
        sleepingThreads::sleepingQueue().subOneTimeSlice();
        sleepingThreads::sleepingQueue().checkToRemoveThread();
        if(PCB::timeSliceCounter>= PCB::currentRunning->getTimeSlice()){
            uint64 volatile sepc = r_sepc();
            uint64 volatile sstatus = r_sstatus();
            mode=1;
            PCB::timeSliceCounter=0;
            PCB::dispatch();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }
        mc_sip(SIP_SSIE);
    }
}
void Riscv::consoleTrapHandler() {
    uint64 volatile scause=r_scause();
    if (scause==0x8000000000000009UL){
        //console_handler();

        int req = plic_claim();
        while(*((char*)CONSOLE_STATUS) & CONSOLE_RX_STATUS_BIT){
            char c = *(char*)CONSOLE_RX_DATA;
            MyBuffer::inputBuffer->putcB(c);
        }
        plic_complete(req);

    }
}

void Riscv::somethingCrazyHandler(){
    uint64 volatile sepc = r_sepc();
    uint64 volatile scause=r_scause();
    printString("scause: ");
    printInt(scause);
    printString("\n");
    printString("sepc: ");
    printInt(sepc, 16);
    printString("\n");
    PCB::setAllFinished();
    PCB::currentRunning->setFinished();
    PCB::resetTimeSliceCounter();
    PCB::dispatch();

}

void *mem_alloc(size_t size){

    uint32 numOfBlocks= ((uint32)size%MEM_BLOCK_SIZE) ? ((uint32)size/MEM_BLOCK_SIZE + 1): ((uint32)size/MEM_BLOCK_SIZE);
    uint8 code=CODE_MEM_ALLOC;
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (code));
    __asm__ volatile ("mv a1, %[numOfBlocks]" : : [numOfBlocks] "r" (numOfBlocks));
    __asm__ volatile ("ecall");
    void *ret;
    __asm__ volatile ("mv %[ret], a0" : [ret] "=r"(ret));
    return ret;
}

int mem_free(void*pointer){
    uint8 code=CODE_MEM_FREE;
    __asm__ volatile ("mv a1, %[pointer]" : : [pointer] "r" (pointer));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (code));
    __asm__ volatile ("ecall");
    int fr;
    __asm__ volatile ("mv %[fr], a0" : [fr] "=r" (fr));
    return fr;
}

int thread_create( thread_t* handle, void(*start_routine)(void* arg), void*arg){
    uint8 code=CODE_THREAD_CREATE;
    char* stack;
    if(start_routine!= nullptr)
    {
        stack =(char*) mem_alloc(8*PCB::stackSize);
    }
    else stack=nullptr;
    __asm__ volatile ("mv a4, %[stack]" : : [stack] "r" (stack));
    __asm__ volatile ("mv a3, %[arg]" : : [arg] "r" (arg));
    __asm__ volatile ("mv a2, %[start]" : : [start] "r" (start_routine));
    __asm__ volatile ("mv a1, %[handle]" : : [handle] "r" (handle));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (code));
    __asm__ volatile ("ecall");

    if(*handle != nullptr) return 0;
     return 0;
}

int thread_exit(){
    if(PCB::currentRunning->isFinished()) return -1;
    uint8 code=CODE_THREAD_EXIT;
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (code));
    __asm__ volatile ("ecall");
    return 0;
}

void thread_dispatch(){
    uint8 code= CODE_THREAD_DISPATCH;
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (code));
    __asm__ volatile ("ecall");
}

void thread_join(thread_t handle){
    uint8 code= CODE_THREAD_JOIN;
    __asm__ volatile ("mv a1, %[handle]" : : [handle] "r" (handle));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (code));
    __asm__ volatile ("ecall");
}


int sem_open(sem_t* handle,unsigned init ){
    uint8 code = CODE_SEM_OPEN;
    __asm__ volatile ("mv a2, %[init]" : : [init] "r" (init));
    __asm__ volatile ("mv a1, %[handle]" : : [handle] "r" (handle));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (code));
    __asm__ volatile ("ecall");
    //if(*handle!= nullptr) return 0;
    //else return -1;
    int ret;
    __asm__ volatile ("mv %[ret], a0" : [ret] "=r" (ret));
    return ret;
}

int sem_close(sem_t handle){
    uint8 code = CODE_SEM_CLOSE;
    __asm__ volatile ("mv a1, %[handle]" : : [handle] "r" (handle));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (code));
    __asm__ volatile ("ecall");
    int ret;
    __asm__ volatile ("mv %[ret], a0" : [ret] "=r" (ret));
    return ret;
}

int sem_wait(sem_t handle){
    uint8 code = CODE_SEM_WAIT;
    __asm__ volatile ("mv a1, %[handle]" : : [handle] "r" (handle));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (code));
    __asm__ volatile ("ecall");
    int ret;
    __asm__ volatile ("mv %[ret], a0" : [ret] "=r" (ret));
    return ret;
}

int sem_signal(sem_t handle){
    uint8 code = CODE_SEM_SIGNAL;
    __asm__ volatile ("mv a1, %[handle]" : : [handle] "r" (handle));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (code));
    __asm__ volatile ("ecall");
    int ret;
    __asm__ volatile ("mv %[ret], a0" : [ret] "=r" (ret));
    return ret;
}

int time_sleep(time_t time){
    if(time==0) return -1;
    uint8 code = CODE_THREAD_SLEEP;
    __asm__ volatile ("mv a1, %[time]" : : [time] "r" (time));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (code));
    __asm__ volatile ("ecall");
    int ret;
    __asm__ volatile ("mv %[ret], a0" : [ret] "=r" (ret));
    return ret;
}

char getc(){
    uint8 code = CODE_GETC;
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (code));
    __asm__ volatile ("ecall");
    char ret;
    __asm__ volatile ("mv %[ret], a0" : [ret] "=r" (ret));
    return ret;
}

void putc(char c){
    uint8 code = CODE_PUTC;
    __asm__ volatile ("mv a1, %[c]" : : [c] "r" (c));
    __asm__ volatile ("mv a0, %[code]" : : [code] "r" (code));
    __asm__ volatile ("ecall");
}



/*

void Riscv:: mem_alloc_handler(){
    uint64 scause=r_scause();
    if(scause == 0x08 || scause==0x09) {
        uint64 volatile sepc = r_sepc();
        uint64 sstatus = r_sstatus();
        size_t size;
        __asm__ volatile ("mv %[size], a1" : [size] "=r"(size));
        MemoryAllocator::instanceMemAlloc().mem_alloc(size);
        w_sepc(sepc + 4);
        w_sstatus(sstatus);
    }
}

void Riscv:: mem_free_handler() {
    uint64 scause = r_scause();
    if (scause == 0x08 || scause == 0x09) {
        uint64 volatile sepc = r_sepc();
        uint64 sstatus = r_sstatus();
        void *p;
        __asm__ volatile ("mv %[p], a1" : [p] "=r"(p));
        MemoryAllocator::instanceMemAlloc().mem_free(p);
        w_sepc(sepc + 4);
        w_sstatus(sstatus);
    }
}
*/


