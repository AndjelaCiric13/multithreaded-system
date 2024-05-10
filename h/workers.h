//
// Created by os on 6/22/23.
//

#ifndef PROJEKAT_WORKERS_H
#define PROJEKAT_WORKERS_H

#include "../h/riscv.hpp"

extern void addIdleThread(void*);
extern void workerBodyA(void*);

extern void workerBodyB(void*);

extern void workerBodyC(void*);
extern void workerBodyD(void*);

#endif //PROJEKAT_WORKERS_H
