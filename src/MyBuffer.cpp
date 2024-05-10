//
// Created by os on 6/29/23.
//

#include "../h/MyBuffer.h"

MyBuffer* ::MyBuffer::outputBuffer=nullptr;
MyBuffer* ::MyBuffer::inputBuffer=nullptr;

MyBuffer::MyBuffer() {
    sem_open(&itemAvailable, 0);
    sem_open(&mutexHead, 1);
    sem_open(&mutexTail, 1);
    head=tail=count=0;
}

MyBuffer::~MyBuffer() {
   sem_close(itemAvailable);
   sem_close(mutexTail);
   sem_close(mutexHead);
}

int MyBuffer::getCnt(){
    return count;
}

void MyBuffer::putcB(char c) {
    Sem::semWait(mutexTail);
    if(count <buff_size){
        buffer[tail] = c;
        tail = (tail+1)%buff_size;
        count++;
        Sem::semSignal(itemAvailable);
    }
   Sem::semSignal(mutexTail);
}

char MyBuffer::getcB() {
    Sem::semWait(itemAvailable);
    Sem::semWait(mutexHead);
    char chr= buffer[head];
    head = (head+1)%buff_size;
    count--;
    Sem::semSignal(mutexHead);
    return chr;
}