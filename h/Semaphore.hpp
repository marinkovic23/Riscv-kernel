#ifndef ___SEMAPHORE_H___
#define ___SEMAPHORE_H___

#include "../lib/hw.h"


class TCB;



typedef struct sem {
    TCB* headOfWaiting;
    TCB* tailOfWaiting;
    int count;
} sem;

typedef sem* sem_t;


class Semaphore {
public:



protected:


private:
    Semaphore();
    ~Semaphore();
    friend class RiscV;
    friend class Buffer;

    static sem* sem_open(sem_t* handle, uint64 init);
    static int sem_close(sem_t handle);

    static int sem_wait(sem_t handle); //we have to test the default values
    static int sem_signal(sem_t handle);

    static int sem_wait_n(sem_t handle, unsigned n);
    static int sem_signal_n(sem_t handle, unsigned n);


    static int sem_trywait(sem_t sem);
    static int sem_trysignal(sem_t sem);

    static void insertIntoBlockQueue(sem_t sem);
    static TCB* removeFromBlockQueue(sem_t sem);





};



#endif