//
// Created by os on 6/26/26.
//

#ifndef RISCVKERNEL_BUFFER_HPP
#define RISCVKERNEL_BUFFER_HPP


#include "Semaphore.hpp"


class Buffer {
public:
    Buffer();

    void putb(char c);
    char getb();

    bool empty();
    bool full();

    void* operator new(size_t size);
    void operator delete(void* ptr);

    bool tryPut(char c);
    bool tryGet(char& c);
private:
    sem_t spaceAvailable;
    sem_t itemAvailable;
    sem_t mutex;
    uint64 head;
    uint64 tail;
    static const uint64 bufferSize = 1024;
    char buffer[bufferSize];



};

inline bool Buffer::empty() {
    return head == tail;
}

inline bool Buffer::full() {
    return (tail + 1) % bufferSize == head;
}



#endif //RISCVKERNEL_BUFFER_HPP
