

#include "../h/syscall_c.h"

void* mem_alloc (size_t size) {

    int syscallCode = 0x01;
    void* returnValue;

    size_t mySize;
    if (size % MEM_BLOCK_SIZE != 0) {
        mySize = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE * MEM_BLOCK_SIZE;
    }
    else mySize = size;

    __asm__ volatile("mv a1, %1\n"
                     "li a0, %2\n"
                     "ecall\n"
                     "mv %0, a0"
                     :"=r"(returnValue)
                     : "r"(mySize), "i"(syscallCode)
                     : "a0", "a1"
                     );

    return returnValue;

}




int mem_free (void* ptr) {

    const int syscallCode = 0x02;

    __asm__ volatile ("mv a1, %0\n"
                      "li a0, %1\n"
                      "ecall"
                      :
                      : "r"(ptr), "i"(syscallCode)
                      : "a0", "a1"
                      );

    uint64 returnValue;

    __asm__ volatile("mv %0, a0" : "=r"(returnValue));

    return (int) returnValue;
}

size_t mem_get_free_space() {
    size_t returnValue;

    const int syscallCode = 0x03;

    __asm__ volatile ("li a0, %1\n"
                      "ecall\n"
                      "mv %0, a0"
                      : "=r"(returnValue)
                      : "i"(syscallCode));

    return returnValue;
}

size_t mem_get_largest_free_block() {
    size_t returnValue;
    const int syscallCode = 0x04;

    __asm__ volatile("li a0, %1\n"
                     "ecall\n"
                     "mv %0, a0"
                     : "=r"(returnValue)
                     : "i"(syscallCode));

    return returnValue;
}


int thread_create (thread_t* handle, void(*start_routine)(void*), void* arg) {
    int returnValue;
    int syscallCode = 0x11;

    __asm__ volatile(
            "mv a1, %1\n"
            "mv a2, %2\n"
            "mv a3, %3\n"
            "li a0, %4\n"

            "ecall\n"

            "mv %0, a0"

            : "=r"(returnValue)
            : "r"(handle), "r"(start_routine), "r"(arg), "i"(syscallCode)
            : "a0", "a1", "a2", "a3", "a4", "memory"
            );

    return returnValue;
}

int thread_exit () {
    int returnValue;

    int syscallCode = 0x12;

    __asm__ volatile (
            "li a0, %1\n"
            "ecall\n"
            "mv %0, a0"
            : "=r"(returnValue)
            : "i"(syscallCode)
            : "a0"
            );

    return returnValue;

}

void thread_dispatch () {
    int returnValue;

    int syscallCode = 0x13;

    __asm__ volatile (
            "li a0, %1\n"
            "ecall\n"
            "mv %0, a0"
            : "=r"(returnValue)
            : "i"(syscallCode)
            : "a0"
            );


}

int sem_open (sem_t* handle, unsigned init) {

    int syscallCode = 0x21;
    int returnValue;

    __asm__ volatile(
            "mv a1, %1\n"
            "mv a2, %2\n"
            "li a0, %3\n"

            "ecall\n"

            "mv %0, a0"

            : "=r"(returnValue)
            : "r"(handle), "r"(init), "i"(syscallCode)
            : "a0", "a1", "a2", "a3", "memory"
            );

    return returnValue;


}

int sem_close (sem_t handle) {

    int syscallCode = 0x22;
    int returnValue;

    __asm__ volatile(
            "mv a1, %1\n"
            "li a0, %2\n"

            "ecall\n"

            "mv %0, a0"

            : "=r"(returnValue)
            : "r"(handle), "i"(syscallCode)
            : "a0", "a1"
            );

    return returnValue;

}

int sem_wait (sem_t id) {

    int syscallCode = 0x23;
    int returnValue;

    __asm__ volatile(
            "mv a1, %1\n"
            "li a0, %2\n"

            "ecall\n"

            "mv %0, a0"

            : "=r"(returnValue)
            : "r"(id), "i"(syscallCode)
            : "a0", "a1"
            );

    return returnValue;

}

int sem_signal (sem_t id) {
    int syscallCode = 0x24;
    int returnValue;

    __asm__ volatile(
            "mv a1, %1\n"
            "li a0, %2\n"

            "ecall\n"

            "mv %0, a0"

            : "=r"(returnValue)
            : "r"(id), "i"(syscallCode)
            : "a0", "a1"
            );

    return returnValue;
}

int sem_wait_n (sem_t id, unsigned n) {
    int returnValue;

    __asm__ volatile(
            "mv a1, %1\n"
            "mv a2, %2\n"
            "li a0, 0x25\n"

            "ecall\n"

            "mv %0, a0"

            : "=r"(returnValue)
            : "r"(id), "r"(n)
            : "a0", "a1", "a2", "memory"
            );

    return returnValue;
}

int sem_signal_n (sem_t id, unsigned n) {
    int returnValue;

    __asm__ volatile(
            "mv a1, %1\n"
            "mv a2, %2\n"
            "li a0, 0x26\n"

            "ecall\n"

            "mv %0, a0"

            : "=r"(returnValue)
            : "r"(id), "r"(n)
            : "a0", "a1", "a2", "memory"
            );

    return returnValue;
}



int time_sleep (time_t duration) {

    int syscallCode = 0x31;
    int returnValue;

    __asm__ volatile(
            "mv a1, %1\n"
            "li a0, %2\n"

            "ecall\n"

            "mv %0, a0"

            : "=r"(returnValue)
            : "r"(duration), "i"(syscallCode)
            : "a0", "a1"
            );

    return returnValue;

}

char getc () {

    int returnValue;

    int syscallCode = 0x41;

    __asm__ volatile (
            "li a0, %1\n"
            "ecall\n"
            "mv %0, a0"
            : "=r"(returnValue)
            : "i"(syscallCode)
            : "a0"
            );

    return returnValue;
}

void putc (char myChar) {

    int syscallCode = 0x42;
    int returnValue;

    __asm__ volatile(
            "mv a1, %1\n"
            "li a0, %2\n"

            "ecall\n"

            "mv %0, a0"

            : "=r"(returnValue)
            : "r"(myChar), "i"(syscallCode)
            : "a1"
            );
}



















