//
// Created by os on 5/22/23.
//

#ifndef PROJEKAT_LISTA_H
#define PROJEKAT_LISTA_H

#include "../h/memory_allocator.h"

template <typename T>class Lista
{
protected:
    struct Elem {
        T pod;
        int redDodavanja;
        Elem* next;
        Elem* prev;
        Elem(const T& p, Elem* sled = nullptr, Elem* pret = nullptr, int red=0) :pod(p), redDodavanja(red), next(sled), prev(pret) {}
    };
    Elem* first = nullptr, * last = nullptr;
    mutable Elem* current = nullptr;
    virtual void brisi();
    void kopiraj(const Lista& l);
    void premesti(Lista& l);
public:
    Lista() {} ;
    Lista(const Lista& l) { kopiraj(l); }
    Lista(Lista&& l) { premesti(l); }
    Lista& operator=(const Lista& l);
    Lista& operator=(Lista&& l);
    virtual ~Lista() { brisi(); }

    int getBrojElemenata()const;
    Lista& dodajNaKraj(const T& podatak);

    void postaviNaPrvi()const { current = first; }
    void naSled()const { if(current) current = current->next; }
    void naPret()const { if(current)current = current->prev; }
    void postaviNaPoslednji()const { current = last; }

    T izbaciTrenutni(){
        current = first;
        first = first->next;
        T tr = current->pod;
        if(current==last) first=last=nullptr;
        MemoryAllocator::instanceMemAlloc().mem_free(current);
        current = first;
        return tr;
    };

    T& dohvatiCurrPod()const;
    T& dohvatiPretPod()const;
    T& dohvatiSledPod()const;
    bool imaTek()const { return current != nullptr; }

    bool imaSled()const { if (current) return current->next != nullptr; }
    bool imaPret()const { if (current) return current->prev != nullptr; }
};

template <typename T>void Lista<T>::brisi() {
    Elem* curr = first;
    while (curr) {
        Elem* old = curr;
        curr = curr->next;
        MemoryAllocator::instanceMemAlloc().mem_free(old);
    }
    first = last =current= nullptr;
}

template <typename T> void Lista<T>::kopiraj(const Lista&l) {
    first = last =current= nullptr;
    for (Elem* curr = l.first; curr; curr = curr->next) {
        Elem* prev = nullptr;
        if (last)  prev = last;
        Elem* newElem = (Elem*)MemoryAllocator::instanceMemAlloc().mem_alloc(
                sizeof(Elem)/ MEM_BLOCK_SIZE + (
                        (sizeof(Elem)% MEM_BLOCK_SIZE>0)?1:0
                )
                );
        newElem->pod=curr->pod;

        last = (!first ? first : last->next) = newElem;
        last->prev = prev;
    }
}

template <typename T> void Lista<T>::premesti(Lista& l) {
    first = l.first; last = l.last; current = l.current; l.first = l.last =l.current= nullptr;
}

template <typename T> Lista<T>& Lista<T>::operator=(const Lista& l) {
    if (this != &l) {
        brisi(); kopiraj(l);
    }
    return *this;
}

template <typename T> Lista<T>& Lista<T>::operator=(Lista&& l) {
    if (this != &l) {
        brisi(); premesti(l);
    }
    return *this;
}

template <typename T> int Lista<T>::getBrojElemenata() const{
    int num = 0;
    Elem* curr = first;
    while (curr) {
        num++; curr = curr->next;
    }
    return num;
}

template <typename T> Lista<T>& Lista<T>::dodajNaKraj(const T&podatak) {
    Elem* prev = nullptr;
    if (last)prev = last;
    last = (!first ? first : last->next) = (Elem*)MemoryAllocator::instanceMemAlloc().mem_alloc(sizeof(Elem)/MEM_BLOCK_SIZE +
            ((sizeof (Elem)%MEM_BLOCK_SIZE>0)?1:0));
    last ->pod = podatak; last->next=nullptr; last->prev = nullptr; last->redDodavanja=getBrojElemenata()+1;
    last->prev = prev;
    if(first== nullptr) first = last;
    return *this;
}

template <typename T> T& Lista<T>::dohvatiCurrPod()const {
    return current->pod;
}

template <typename T> T& Lista<T>::dohvatiPretPod()const {
    return current->prev->pod;
}

template <typename T> T& Lista<T>::dohvatiSledPod()const {
    return current->next->pod;
}


#endif //PROJEKAT_LISTA_H
