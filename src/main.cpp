#include "../h/riscv.hpp"
#include "../h/Thread.hpp"
#include "../h/syscall_c.h"

#include "../h/KConsole.h"


void userMain();

sem_t userMainSem;

void userMainWrapper(void* arg) {
    userMain();
    sem_signal(userMainSem);
}

void idleBody(void*) {
    while (true) {
        __asm__ __volatile__("csrsi sstatus, 0x2" ::: "memory");
        __asm__ __volatile__("wfi" ::: "memory");
    }

}

void createMainThread() {
    TCB* mainThread = (TCB*)mem_alloc(sizeof(TCB));
    Context* ctx = (Context*)mem_alloc(sizeof(Context));

    mainThread->ctx = ctx;
    mainThread->start = nullptr;
    mainThread->arg = nullptr;
    mainThread->stackBase = nullptr;
    mainThread->threadState = RUNNING;
    mainThread->next = nullptr;
    mainThread->prev = nullptr;

    mainThread->semWaitAmount = 0;
    mainThread->timeSlice = DEFAULT_TIME_SLICE;

    Scheduler::currentThread = mainThread;
}



void init_stvec() {
    __asm __volatile("csrw stvec, %0":: "r"(&supervisor_trap));

    /*__asm __volatile("mv t0, %0":: "r"(&supervisor_trap));
    __asm __volatile("addi t0, t0, 1");
    __asm __volatile ("csrw stvec, t0");*/ //may be done later

}




int main() {

    /*uint64 seie = 1UL << 9;
    __asm__ __volatile__("csrw sie, %0" :: "r"(seie) : "memory");*/ //sada kada imamo tajmer nema potrebe da disablujemo


    init_stvec();

    MemoryAllocator::init();

    KConsole::init();



    createMainThread();


    thread_t idle;
    KThread::thread_create(&idle, idleBody, nullptr, KERNEL_THREAD, false);

    Scheduler::idleThread = idle;




    sem_open(&userMainSem, 0);

    thread_t user;

    thread_create(&user, userMainWrapper, nullptr);

    sem_wait(userMainSem);

    return 0;

}




