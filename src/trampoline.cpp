//
// Created by Andrija Marinkovic on 21. 1. 2026..
//
#include "../h/Thread.hpp"
#include "../h/syscall_c.h"

extern "C" void enter_user_mode(void(*body)(void*), void* arg);

void thread_trampoline() {
    TCB* t = Scheduler::currentThread;

    if (t->mode == USER_THREAD) {
        enter_user_mode(t->start, t->arg);
        while (true) {}
    }

    //kernel thread
    if (t->start != nullptr) {
        t->start(t->arg);
    }

    KThread::thread_exit();

    while (true) {}
}

extern "C" void user_thread_wrapper(void(*body)(void*), void* arg) {
    if (body != nullptr) {
        body(arg);
    }

    thread_exit();
    while (true);
}


/*void thread_trampoline(void* threadPtr) {
    TCB* t = Scheduler::currentThread;
    if (t->start != nullptr) {
        t->start(t->arg);
    }
    thread_exit();
}*/
