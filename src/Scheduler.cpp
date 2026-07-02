#include "../h/Scheduler.hpp"

#include "../h/Thread.hpp"
#include "../h/riscv.hpp"

//ready queue is first come first serve in this kernel
TCB* Scheduler::headOfReady = nullptr;
TCB* Scheduler::tailOfReady = nullptr;
TCB* Scheduler::currentThread = nullptr;
time_t Scheduler::remainingTime = 0;

TCB* Scheduler::headOfSleeping = nullptr;
TCB* Scheduler::tailOfSleeping = nullptr;

TCB* Scheduler::idleThread = nullptr;

TCB* Scheduler::get() {
    if (headOfReady == nullptr) {
        return nullptr;
    }
    TCB* returnValue = headOfReady;
    headOfReady = headOfReady->next;

    if (headOfReady != nullptr) {
        headOfReady->prev = nullptr;
    }
    else {
        tailOfReady = nullptr;
    }
    returnValue->next = nullptr;
    returnValue->prev = nullptr;
    returnValue->threadState = RUNNING;

    return returnValue;
}

void Scheduler::put(TCB* myTCB) {

    if (myTCB == nullptr) return;

    myTCB->next = nullptr;
    myTCB->threadState = READY;

    if(headOfReady == nullptr) {
        myTCB->prev = nullptr;
        headOfReady = myTCB;
        tailOfReady = myTCB;
    }
    else {
        myTCB->prev = tailOfReady;
        tailOfReady->next = myTCB;
        tailOfReady = myTCB;
    }


}

void Scheduler::contextSwitch(Context* old, Context* _new) {
    context_switch(old, _new);
}

void Scheduler::wakeSleepingThreads(uint64 currentTime) {
    while (headOfSleeping != nullptr && headOfSleeping->wakeTime <= RiscV::timeCounter) {
        TCB* t = headOfSleeping;

        headOfSleeping = headOfSleeping->next;

        if (headOfSleeping != nullptr) {
            headOfSleeping->prev = nullptr;
        }
        else {
            tailOfSleeping = nullptr;
        }


        t->next = nullptr;
        t->prev = nullptr;
        t->threadState = READY;

        Scheduler::put(t);
    }
}

void Scheduler::putSleeping(TCB* t) {
    if (t == nullptr) return;

    t->threadState = SLEEPING;
    t->next = nullptr;
    t->prev = nullptr;

    //empty sleeping list
    if (headOfSleeping == nullptr) {
        headOfSleeping = t;
        tailOfSleeping = t;
        return;
    }

    //insert before current head
    if (t->wakeTime < headOfSleeping->wakeTime) {
        t->next = headOfSleeping;
        headOfSleeping->prev = t;
        headOfSleeping = t;
        return;
    }

    //find insertion point
    TCB* curr = headOfSleeping;

    while (curr->next != nullptr && curr->next->wakeTime <= t->wakeTime) {
        curr = curr->next;
    }

    //insert after curr
    t->next = curr->next;
    t->prev = curr;

    if (curr->next != nullptr) {
        curr->next->prev = t;
    }
    else {
        tailOfSleeping = t;
    }

    curr->next = t;
}




