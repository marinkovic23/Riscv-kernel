#include "../h/Semaphore.hpp"

#include "../h/MemoryAllocator.hpp"

#include "../h/Scheduler.hpp"

#include "../h/Thread.hpp"





void Semaphore::insertIntoBlockQueue(sem_t sem) {
    Scheduler::currentThread->threadState = BLOCKED;
    TCB* toBeBlocked = Scheduler::currentThread;

    if (sem->headOfWaiting == sem->tailOfWaiting) {
        sem->headOfWaiting = toBeBlocked;
        sem->tailOfWaiting = toBeBlocked;
    }

    else {
        sem->tailOfWaiting->next = toBeBlocked;
        toBeBlocked->prev = sem->tailOfWaiting;
        sem->tailOfWaiting = toBeBlocked;
    }


}

sem* Semaphore::sem_open(sem_t* handle, uint64 init) {

    if (handle == nullptr) {
        return nullptr;
    }



    sem* mySem =(sem*) MemoryAllocator::mem_alloc(sizeof(sem));
    if (mySem == nullptr) {
        *handle = nullptr;
        return nullptr;
    }


    TCB* headOfWaitQueue = nullptr;
    TCB* tailOfWaitQueue = headOfWaitQueue;
    mySem->headOfWaiting = headOfWaitQueue;
    mySem->tailOfWaiting = tailOfWaitQueue;
    mySem->count = init;

    *handle = mySem;



    return mySem;

}

int Semaphore::sem_close(sem* sem) {
    if (sem == nullptr) return -1;

    while (sem->headOfWaiting != nullptr) {
        TCB* t = sem->headOfWaiting;

        sem->headOfWaiting = t->next;

        if (sem->headOfWaiting != nullptr) {
            sem->headOfWaiting->prev = nullptr;
        }
        else {
            sem->tailOfWaiting = nullptr;
        }

        t->next = nullptr;
        t->prev = nullptr;
        t->semWaitAmount = 0;
        t->threadState = READY;
        t->freedLegitimately = false;

        Scheduler::put(t);
    }

    int returnValue = MemoryAllocator::mem_free(sem);

    return returnValue;
}

int Semaphore::sem_wait(sem_t sem) {
    return Semaphore::sem_wait_n(sem, 1);
}

int Semaphore::sem_signal(sem* sem) { //have to see the arguments
    return Semaphore::sem_signal_n(sem, 1);
}

int Semaphore::sem_trywait(sem_t sem) {
    if (sem == nullptr) {
        return -1;
    }

    if (sem->count <= 0) {
        return -1;
    }
    sem->count--;
    return 0;
}

int Semaphore::sem_wait_n(sem_t sem, unsigned n) {
    if (sem == nullptr) {
        return -1;
    }

    if (n == 0) return 0;

    if (sem->count >=(int) n) {
        sem->count -= n;
        return 0;
    }

    TCB* current = Scheduler::currentThread;

    current->threadState = BLOCKED;
    current->next = nullptr;
    current->prev = nullptr;
    //current->freedLegitimately = false;
    current->semWaitAmount = n;

    if (sem->headOfWaiting == nullptr) {
        sem->headOfWaiting = sem->tailOfWaiting = current;
    }
    else {
        current->prev = sem->tailOfWaiting;
        sem->tailOfWaiting->next = current;
        sem->tailOfWaiting = current;
    }

    KThread::thread_dispatch();

    current->semWaitAmount = 0;

    return current->freedLegitimately ? 0 : -1;
}

int Semaphore::sem_signal_n(sem_t sem, unsigned n) {
    if (sem == nullptr) {
        return -1;
    }

    if (n == 0) return 0;

    sem->count += n;

    while (sem->headOfWaiting != nullptr) {
        TCB* t = sem->headOfWaiting;

        if (sem->count < t->semWaitAmount) {
            break;
        }

        sem->count -= t->semWaitAmount;

        sem->headOfWaiting = t->next;

        if (sem->headOfWaiting != nullptr) {
            sem->headOfWaiting->prev = nullptr;
        }
        else {
            sem->tailOfWaiting = nullptr;
        }

        t->next = nullptr;
        t->prev = nullptr;
        t->threadState = READY;
        t->freedLegitimately = true;

        Scheduler::put(t);
    }

    return 0;
}