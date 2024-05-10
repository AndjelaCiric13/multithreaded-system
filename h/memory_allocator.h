//
// Created by os on 5/11/23.
//

#ifndef memory_allocator
#define memory_allocator

#include "../lib/hw.h"

class MemoryAllocator{
private:
    typedef struct FreeMemDesc{
        FreeMemDesc* next;
        FreeMemDesc*prev;
        uint8 used;
        size_t size;
    }FreeMemDesc;

    static int totalFree;
    static int freeMemHeadInit;
    static void memcpy(void*dst, void *src, size_t);
    static void reallocate();
    static void initFreeHead();
    MemoryAllocator(){};

protected:
    static FreeMemDesc* head;
public:
    static MemoryAllocator& instanceMemAlloc(){
        static MemoryAllocator Instance;
        return Instance;
    }
    void *mem_alloc (size_t size);
    int mem_free (void*);
};

#endif //PROJEKAT_MEMORY_ALLOCATOR_H
