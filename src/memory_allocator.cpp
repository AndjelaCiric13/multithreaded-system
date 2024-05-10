//
// Created by os on 5/11/23.
//

#include "../h/memory_allocator.h"
constexpr int NoFreeMem=1;
constexpr int NotPreviousAllocated=2;
int MemoryAllocator::freeMemHeadInit=0;
int MemoryAllocator::totalFree=0;
MemoryAllocator::FreeMemDesc* MemoryAllocator::head=nullptr;

void MemoryAllocator::memcpy(void *dst, void *src, size_t n) {
    char* dstC=(char*) dst;
    char* srcC=(char*) src;
    for(size_t i=0; i<n;i++){
        dstC[i]=srcC[i];
    }
}

void MemoryAllocator::initFreeHead() {
    freeMemHeadInit=1;
    head=(FreeMemDesc*) HEAP_START_ADDR;
    int addToDesc=(sizeof(FreeMemDesc)%MEM_BLOCK_SIZE)?(MEM_BLOCK_SIZE - sizeof(FreeMemDesc)%MEM_BLOCK_SIZE) : 0;
    int sizeOfDesc=sizeof(FreeMemDesc)+addToDesc;
    head->size=(char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR - sizeOfDesc;
    head->next=nullptr; head->prev=nullptr;
    head->used=0;
    totalFree=head->size;
}

void *MemoryAllocator::mem_alloc(size_t size) {
    void* allocated=nullptr;
    if(size<=0) return allocated;
    if(!freeMemHeadInit) initFreeHead();
    //size_t add = (size%MEM_BLOCK_SIZE)?(MEM_BLOCK_SIZE - size%MEM_BLOCK_SIZE) : 0;
    //size_t allocateSize= size + add;
    size_t allocateSize= size*MEM_BLOCK_SIZE;
    int addToDesc=(sizeof(FreeMemDesc)%MEM_BLOCK_SIZE)?(MEM_BLOCK_SIZE - sizeof(FreeMemDesc)%MEM_BLOCK_SIZE) : 0;
    int sizeOfDesc=sizeof(FreeMemDesc)+addToDesc;

    if(totalFree <  (int)allocateSize) return allocated;
    FreeMemDesc * curr=head;
    FreeMemDesc* bestFit =nullptr;
    int minDiff=totalFree;
    for(;curr;curr=curr->next){
        if(curr->used) continue;
        if(curr->size<allocateSize) continue;
        int currDiff=curr->size - allocateSize;
        currDiff=(currDiff>0)? currDiff:(-currDiff);
        if(minDiff>=currDiff) {
            minDiff=currDiff;
            bestFit=curr;
        }
    }
    if(!bestFit) {
        return nullptr;
        //FreeMemDesc *copyHead=head;
        //reallocate();
        //bestFit=head;
        //head=copyHead;
    }
    FreeMemDesc*newDesc=nullptr;
    int createNewFree=0;
    if(minDiff>(int)(sizeOfDesc+MEM_BLOCK_SIZE)) createNewFree=1;
    if(createNewFree) {
        newDesc = (FreeMemDesc*)((char*)bestFit + sizeOfDesc + allocateSize);
        bestFit->size-=((!bestFit->next)?(char*)HEAP_END_ADDR:(char*)bestFit->next) - (char*)newDesc;
        newDesc->size=((!bestFit->next)?(char*)HEAP_END_ADDR:(char*)bestFit->next) - (char*)newDesc - sizeOfDesc;
        if(bestFit && bestFit->next) bestFit->next->prev=newDesc;
        newDesc->next=bestFit->next;
        newDesc->prev=bestFit;
        bestFit->next=newDesc;
        newDesc->used=0;
        totalFree-=sizeOfDesc;
        //int m=sizeof(FreeMemDesc);
    }
    totalFree-=bestFit->size;
    bestFit->used=1;
    //char* pointer=(char*)((char*)bestFit + sizeof(FreeMemDesc));
    return (void*)((char*)bestFit + sizeOfDesc);
}

void MemoryAllocator::reallocate() {
    int addToDesc=(sizeof(FreeMemDesc)%MEM_BLOCK_SIZE)?(MEM_BLOCK_SIZE - sizeof(FreeMemDesc)%MEM_BLOCK_SIZE) : 0;
    int sizeOfDesc=sizeof(FreeMemDesc)+addToDesc;
    char* start=(char*) HEAP_START_ADDR;
    ((FreeMemDesc *)start)->prev=nullptr;
    FreeMemDesc*curr=head;
    size_t totalUsed=0;
    FreeMemDesc *prev=nullptr;
    for(;curr;curr=curr->next) {
        if (curr->used) break;
    }
    while(curr){
        if(curr->used){
            FreeMemDesc *copy=curr;
            size_t copySize=copy->size;
            FreeMemDesc *nextUsed= curr->next;
            while(nextUsed){
                if(nextUsed->used) break;
                nextUsed=nextUsed->next;
            }
            memcpy((char*)start,(char*)copy, sizeOfDesc + copy->size);
            if(nextUsed)((FreeMemDesc*)start)->next = (FreeMemDesc*)((char*) start + sizeOfDesc + ((FreeMemDesc*)start)->size);
            ((FreeMemDesc*)start)->prev=prev;
            ((FreeMemDesc*)start)->used=0;
            prev=(FreeMemDesc*) start;
            start=(char*) (start + sizeOfDesc + copySize);
            totalUsed+=sizeOfDesc + copySize;
            if(!nextUsed) break;
            curr=nextUsed;
            continue;
        }
    }
    head=(FreeMemDesc*)start;
    head->next=nullptr;
    head->size=(char*)HEAP_END_ADDR - (char*)head-sizeOfDesc;
    totalFree=head->size;
}

int MemoryAllocator::mem_free(void *p) {
    if(p==nullptr || (char*)p< (char*)HEAP_START_ADDR || (char*)p>= (char*)HEAP_END_ADDR ) return -1;
    FreeMemDesc *curr=head;
    int found=0;
    int addToDesc=(sizeof(FreeMemDesc)%MEM_BLOCK_SIZE)?(MEM_BLOCK_SIZE - sizeof(FreeMemDesc)%MEM_BLOCK_SIZE) : 0;
    int sizeOfDesc=sizeof(FreeMemDesc)+addToDesc;
    while(curr){
        if(curr->used && (char*)p==(char*)((char*) curr + sizeOfDesc)){
            found=1;
            break;
        }
        curr=curr->next;
    }
    if(!found) return -1;
    curr->used=0;
    totalFree+=curr->size;

    if(curr && curr->prev && !curr->prev->used){
        totalFree+=sizeOfDesc;
        curr->prev->size+=curr->size+sizeOfDesc;
        curr->prev->next=curr->next;
        if(curr->next) curr->next->prev=curr->prev;
        curr=curr->prev;
    }
    if(curr && curr->next && !curr->next->used) {
        totalFree+=sizeOfDesc;
        curr->size += curr->next->size + sizeOfDesc;
        if (curr->next->next) {
            curr->next->next->prev = curr;
        }
        curr->next = curr->next->next;
    }
    return 0;
}
