//
// Created by os on 6/29/23.
//

#ifndef PROJEKAT_OS_27_6_MYBUFFER_H
#define PROJEKAT_OS_27_6_MYBUFFER_H
#include "../h/syscall_c.h"
#include "../h/_syscall_cpp.h"

class MyBuffer {

public:
    MyBuffer();
    ~MyBuffer();
    MyBuffer(const MyBuffer&m)=delete;
    MyBuffer(MyBuffer&&m)=delete;
    void operator=(const MyBuffer&m)=delete;
    void operator=(MyBuffer&& m)=delete;
    char getcB();
    void putcB(char c);
    static MyBuffer*outputBuffer;
    static MyBuffer*inputBuffer;
    int getCnt();
private:
    sem_t itemAvailable, mutexHead, mutexTail;
    size_t head, tail, count;
    char buffer[buff_size];
};


#endif //PROJEKAT_OS_27_6_MYBUFFER_H
