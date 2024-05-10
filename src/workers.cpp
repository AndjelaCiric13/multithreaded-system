//
// Created by os on 6/22/23.
//


#include "../lib/hw.h"
#include "../h/PCB.h"
#include "../h/printing.hpp"
#include "../h/riscv.hpp"
#include "../lib/console.h"

extern Sem*sem;

void addIdleThread(void*){
    while(true){
        thread_dispatch();

       //putc('H');
    }
}

void workerBodyA(void*)
{
    for (uint64 i = 0; i < 10; i++)
    {
        printString("A: i=");
        printInt(i);
        printString("\n");
        time_sleep(30);
        for (uint64 j = 0; j < 10000; j++)
        {
            for (uint64 k = 0; k < 30000; k++)
            {
                // busy wait
            }
            //PCB::yield();
        }
    }
    sem_signal(sem);
    //putc('H');
}

void workerBodyB(void*)
{
    for (uint64 i = 0; i < 16; i++)
    {
        printString("B: i=");
        printInt(i);
        printString("\n");
        for (uint64 j = 0; j < 10000; j++)
        {
            for (uint64 k = 0; k < 30000; k++)
            {
                // busy wait
            }
           //PCB::yield();
        }
    }
}

static uint64 fibonacci(uint64 n)
{
    if (n == 0 || n == 1) { return n; }
    if (n % 10 == 0) {
        //Riscv::mode =0;
        PCB::yield();
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

void workerBodyC(void*)
{
    uint8 i = 0;
    for (; i < 3; i++)
    {
        printString("C: i=");
        printInt(i);
        printString("\n");
    }

    printString("C: yield\n");
    __asm__ ("li t1, 7");

    PCB::yield();

    uint64 t1 = 0;
    __asm__ ("mv %[t1], t1" : [t1] "=r"(t1));

    printString("C: t1=");
    printInt(t1);
    printString("\n");

    uint64 result = fibonacci(12);
    printString("C: fibonaci=");
    printInt(result);
    printString("\n");

    for (; i < 6; i++)
    {
        printString("C: i=");
        printInt(i);
        printString("\n");
    }
    //PCB::yield();
}

void workerBodyD(void*)
{
    uint8 i = 10;
    for (; i < 13; i++)
    {
        printString("D: i=");
        printInt(i);
        printString("\n");
    }

    printString("D: yield\n");
    __asm__ ("li t1, 5");
    PCB::yield();

    uint64 result = fibonacci(16);
    printString("D: fibonaci=");
    printInt(result);
    printString("\n");

    for (; i < 16; i++)
    {
        printString("D: i=");
        printInt(i);
        printString("\n");
    }
    //PCB::yield();
}