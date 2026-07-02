#ifndef __SCHEDULER_HPP__
#define __SCHEDULER_HPP__

#include "../lib/hw.h"

class Semaphore;

class Thread;

typedef struct Context Context;

extern "C" void context_switch(Context* oldCtx, Context* newCtx);

typedef struct TCB TCB;

typedef unsigned long time_t;

typedef struct {
    Thread* curr;
    Thread* next;
    Thread* prev;
} element;

class Scheduler {

public:

    static TCB* get();
    static TCB* currentThread;

    static TCB* idleThread;

protected:


private:
    friend class Semaphore;
    friend class KThread;
    friend class RiscV;

	Scheduler();
	~Scheduler();

    static void put(TCB* myTCB);






    static void contextSwitch(Context* old, Context* _new);



    //u vezi sa thread sleep

    static time_t remainingTime; //variable is static because there is only one running thread


    static TCB* headOfReady;
    static TCB* tailOfReady;

    static TCB* headOfSleeping;
    static TCB* tailOfSleeping;



    static void putSleeping(TCB* t);


    static void wakeSleepingThreads(uint64 currentTime);








};


#endif
