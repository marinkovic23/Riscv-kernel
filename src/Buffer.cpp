#include "../h/Buffer.hpp"

#include "../h/MemoryAllocator.hpp"

void* Buffer::operator new(size_t size) {
    return MemoryAllocator::mem_alloc(size);
}

void Buffer::operator delete(void* ptr) {
    MemoryAllocator::mem_free(ptr);
}

Buffer::Buffer() {
    head = tail = 0;

    Semaphore::sem_open(&spaceAvailable, bufferSize);

    Semaphore::sem_open(&itemAvailable, 0);

    Semaphore::sem_open(&mutex, 1);

}

void Buffer::putb(char c) {
    Semaphore::sem_wait(spaceAvailable);
    Semaphore::sem_wait(mutex);
    buffer[tail] = c;
    tail = (tail + 1) % bufferSize;
    Semaphore::sem_signal(mutex);
    Semaphore::sem_signal(itemAvailable);
}


char Buffer::getb() {
    Semaphore::sem_wait(itemAvailable);
    Semaphore::sem_wait(mutex);
    char c= buffer[head];
    head = (head + 1) % bufferSize;
    Semaphore::sem_signal(mutex);
    Semaphore::sem_signal(spaceAvailable);
    return c;
}

bool Buffer::tryPut(char c) {
    if (Semaphore::sem_trywait(spaceAvailable) < 0) {
        return false;
    }

    if (Semaphore::sem_trywait(mutex) < 0) {
        Semaphore::sem_signal(spaceAvailable);
        return false;
    }

    buffer[tail] = c;
    tail = (tail + 1) % bufferSize;

    Semaphore::sem_signal(mutex);
    Semaphore::sem_signal(itemAvailable);

    return true;
}

bool Buffer::tryGet(char& c) {
    if (Semaphore::sem_trywait(itemAvailable) < 0) {
        return false;
    }

    if (Semaphore::sem_trywait(mutex) < 0) {
        Semaphore::sem_signal(itemAvailable);
        return false;
    }

    c = buffer[head];
    head = (head + 1) % bufferSize;

    Semaphore::sem_signal(mutex);
    Semaphore::sem_signal(spaceAvailable);
    return true;
}
