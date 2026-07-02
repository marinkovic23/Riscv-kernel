#include "../h/Thread.hpp"
#include "../h/riscv.hpp"


TCB* KThread::thread_create(TCB** myHandle, void(*startRoutine)(void*), void* startArg, ThreadMode mode, bool putInScheduler) {
    void* stackSpace = MemoryAllocator::mem_alloc(DEFAULT_STACK_SIZE);
    if (stackSpace == nullptr) return nullptr;


	TCB* newTCB = (TCB*) MemoryAllocator::mem_alloc(sizeof(TCB));
    if (newTCB == nullptr) return nullptr;

    Context* ctx = (Context*) MemoryAllocator::mem_alloc(sizeof(Context));
    if (ctx == nullptr) return nullptr;

    *myHandle = newTCB;

    newTCB->ctx = ctx;

	newTCB->start = startRoutine;
	newTCB->arg = startArg;
	newTCB->stackBase = stackSpace;
    newTCB->mode = mode;

    newTCB->threadState = READY;
    newTCB->next = nullptr;
    newTCB->prev = nullptr;

    newTCB->sleepingNext = nullptr;
    newTCB->sleepingPrev = nullptr;

    newTCB->semWaitAmount = 0;

    newTCB->timeSlice = DEFAULT_TIME_SLICE;

    uint64 stackTop = (uint64) stackSpace + DEFAULT_STACK_SIZE;
    stackTop &= -0xFUL;

    ctx->ra = (uint64) &thread_trampoline;
    ctx->sp = stackTop;

    if (putInScheduler) {
        Scheduler::put(newTCB);
    }


    return newTCB;
}

void KThread::thread_dispatch() {
    TCB* oldThread = Scheduler::currentThread;

    if (oldThread != nullptr &&
        oldThread != Scheduler::idleThread &&
        oldThread->threadState != BLOCKED &&
        oldThread->threadState != FINISHED &&
        oldThread->threadState != SLEEPING) {
        Scheduler::put(oldThread);
    }

    TCB* nextToRun = Scheduler::get();

    if (nextToRun == nullptr) {
        nextToRun = Scheduler::idleThread;
    }


    Scheduler::currentThread = nextToRun;

    RiscV::runningTime = 0;

    if (oldThread != nextToRun) {
        Scheduler::contextSwitch(oldThread->ctx, nextToRun->ctx);
    }

}

int KThread::thread_exit() {
    TCB* oldThread = Scheduler::currentThread;
    oldThread->threadState = FINISHED;

    TCB* nextToRun = Scheduler::get();

    if (nextToRun == nullptr) {
        while (true) {}
    }

    Scheduler::currentThread = nextToRun;
    Scheduler::contextSwitch(oldThread->ctx, nextToRun->ctx);


    int returnValue1 = MemoryAllocator::mem_free(oldThread->stackBase);
    int returnValue2 = MemoryAllocator::mem_free(oldThread);



    if (returnValue1 != 0) return returnValue1;
    else return returnValue2;

}

TCB* KThread::createKernelThread(TCB** handle, void (*body)(void*), void* arg) {
    return KThread::thread_create(handle, body, arg, KERNEL_THREAD, true);
}

TCB* KThread::createUserThread(TCB** handle, void (*body)(void*), void* arg) {
    return KThread::thread_create(handle, body, arg, USER_THREAD, true);
}

int KThread::time_sleep(time_t duration) {
    if (duration == 0) return 0;
    TCB* current = Scheduler::currentThread;

    current->wakeTime = RiscV::timeCounter + duration;
    current->threadState = SLEEPING;

    Scheduler::putSleeping(current);

    KThread::thread_dispatch();

    return 0;
}