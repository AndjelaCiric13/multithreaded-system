//
// Created by os on 5/11/23.
//
/*
using size_t = decltype(sizeof(0));

void *operator new(uint64 n)
{
    return __mem_alloc(n);
}

void *operator new[](uint64 n)
{
    return __mem_alloc(n);
}

void operator delete(void *p) noexcept
{
__mem_free(p);
}

void operator delete[](void *p) noexcept
{
__mem_free(p);
}
*/