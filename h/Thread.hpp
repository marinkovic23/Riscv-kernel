
#ifndef __THREAD_HPP__
#define __THREAD_HPP__

#include "../h/Scheduler.hpp"
#include "../h/MemoryAllocator.hpp"

extern "C" void halt();

extern "C" void thread_trampoline();


enum state {READY, RUNNING, BLOCKED, SLEEPING, FINISHED};

enum ThreadMode {
	KERNEL_THREAD,
	USER_THREAD
};




typedef struct Context {

	uint64 ra = (uint64) &thread_trampoline;

	uint64 sp;

	uint64 s0 = 0, s1 = 0, s2 = 0, s3 = 0, s4 = 0, s5 = 0, s6 = 0, s7 = 0, s8 = 0, s9 = 0, s10 = 0, s11 = 0;


} Context;

typedef struct TCB {
	Context* ctx;
	enum state threadState = READY;
	int id;
  	void* stackBase;
 	void (*start)(void*);
    void* arg;
    TCB* next = nullptr;
    TCB* prev = nullptr;
    bool freedLegitimately = true;

	ThreadMode mode;

	uint64 wakeTime;
	TCB* sleepingNext;
	TCB* sleepingPrev;

	int semWaitAmount;
	time_t timeSlice;
} TCB;







class KThread {

public:

	static TCB* thread_create(TCB** myHandle, void(*startRoutine)(void*), void* startArg, ThreadMode mode, bool putInScheduler);

    static int thread_exit();

	static TCB* createKernelThread(TCB** handle, void (*body)(void*), void* arg);

	static TCB* createUserThread(TCB** handle, void (*body)(void*), void* arg);
protected:
private:

    friend class RiscV;
    friend class Scheduler;
    friend class Semaphore;



    static void thread_dispatch ();

    static int time_sleep(time_t duration);




	KThread();
	~KThread();

    //Thread(Thread** myHandle, void(*startRoutine)(void*), void* arg, void* stackSpace);
};




/*inline Thread::Thread(Thread** myHandle, void(*startRoutine)(void*), void* arg, void* stackSpace) {

    this->myHandle = myHandle;

    this->myStartRoutine = startRoutine;
    this->myArg = arg;
    this->sp = stackSpace;
    this->stack = stackSpace;
}*/



#endif