#ifndef riscv
#define riscv
#include "../lib/hw.h"


extern "C" void supervisor_trap();

class RiscV {

public:

    static uint64 timeCounter;
    static uint64 runningTime;


    static void handleTrap(uint64* regs);

    static void handleConsoleTrap();

private:

    static void handleSysCall(uint64* regs);


    RiscV() {}










};




#endif
