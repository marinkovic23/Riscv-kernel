#include "../h/syscall_cpp.hpp"

#include "../h/Thread.hpp"


void* operator new (size_t size) {
    return mem_alloc(size);
}

void operator delete (void* ptr) noexcept {
    mem_free(ptr);
}

void* operator new[](size_t size) {
    return mem_alloc(size);
}

void operator delete [](void* ptr) noexcept {
    mem_free(ptr);
}


void Thread::wrapper(void* arg) {
    Thread* t = (Thread*) arg;
    t->run();
}

Thread::Thread() : myHandle(nullptr), body(&Thread::wrapper), arg(this) {

}

Thread::Thread(void (*body)(void*), void* arg) : myHandle(nullptr), body(body), arg(arg) {}

Thread::~Thread() {}


int Thread::start() {
    return thread_create(&this->myHandle, this->body, this->arg);
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t duration) {
    return time_sleep(duration);
}



Semaphore::Semaphore(unsigned init) : myHandle(nullptr) {

    sem_open(&this->myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(this->myHandle);
}

int Semaphore::wait() {
    return sem_wait(this->myHandle);
}

int Semaphore::signal() {
    return sem_signal(this->myHandle);
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}

PeriodicThread::PeriodicThread (time_t period) : Thread(), period(period) {}

void PeriodicThread::terminate () {
    period = 0;
}

void PeriodicThread::run () {
    while (period != 0) {
        periodicActivation();

        if (period == 0) {
            break;
        }
        time_sleep(period);
    }
}







