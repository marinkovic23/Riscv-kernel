
#include "../h/riscv.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/Thread.hpp"
#include "../h/Semaphore.hpp"
#include "../h/Scheduler.hpp"
#include "../h/KConsole.h"

#include "../h/Buffer.hpp"


uint64 RiscV::timeCounter = 0;
uint64 RiscV::runningTime = 0;


extern "C" void trap_handler(uint64* regs) {
	RiscV::handleTrap(regs);
}

enum RegIndex {
	A0 = 10,
	A1 = 11,
	A2 = 12,
	A3 = 13,
	A4 = 14
};

void RiscV::handleTrap(uint64* regs) {

	uint64 scause;
	uint64 stval;

	uint64 saved_sepc;
	uint64 saved_sstatus;



	uint64 mask;

	__asm__ volatile(
					"csrr %0, scause\n"
					"csrr %1, stval\n"
					"csrr %2, sepc\n"
					"csrr %3, sstatus\n"
					:"=r"(scause), "=r"(stval) ,"=r"(saved_sepc), "=r"(saved_sstatus)
					:
					: "memory"
					);

	uint64 return_sepc = saved_sepc;


	bool shouldDispatch = false;

	switch (scause) {

		case 2:
			//illegal instruction
			KThread::thread_exit();
			break;


		case 8: //ecall from user mode
		case 9: //ecall from system mode
			RiscV::handleSysCall(regs);
			return_sepc = saved_sepc + 4;
			break;

		case 0x8000000000000001:
			mask = 1UL << 1;
			__asm__ __volatile__("csrc sip, %0" :: "r"(mask) : "memory");

			timeCounter++;
			Scheduler::wakeSleepingThreads(timeCounter);

			runningTime++;



			if (Scheduler::currentThread == Scheduler::idleThread) {
				if (Scheduler::headOfReady != nullptr) {
					shouldDispatch = true;
				}
			}
			else if (Scheduler::currentThread != nullptr && runningTime >= Scheduler::currentThread->timeSlice) {
				shouldDispatch = true;
			}
			if (shouldDispatch) {
				runningTime = 0;
				KThread::thread_dispatch();
			}

			KThread::thread_dispatch();
			break;

		case 0x8000000000000009:
			RiscV::handleConsoleTrap();

			break;

		default:
			while (true) {}
	}
	__asm__ __volatile__("csrw sepc, %0" :: "r"(return_sepc): "memory");
	__asm__ __volatile__("csrw sstatus, %0" :: "r"(saved_sstatus): "memory");




    


}


void RiscV::handleSysCall(uint64* regs) {
	uint64 syscallCode = regs[A0];

	switch (syscallCode) {
		case 0x01:
        {
            size_t size = (size_t)regs[A1];

            void* returnValue = MemoryAllocator::mem_alloc(size);
            regs[A0] = (uint64) returnValue;
            break;
        }
		case 0x02:
        {
            void* ptr = (void*) regs[A1];

            int returnValue = MemoryAllocator::mem_free(ptr);
			regs[A0] = (uint64) returnValue;
            break;
        }
		case 0x03:
        {
            size_t returnValue = MemoryAllocator::mem_get_free_space();
			regs[A0] = (uint64) returnValue;
            break;
        }

		case 0x04:
        {
            size_t returnValue = MemoryAllocator::mem_get_largest_free_block();
			regs[A0] = (uint64) returnValue;
            break;
        }


		case 0x11:
        {

			TCB** myHandle = (TCB**)regs[A1];
			void(*start_routine)(void*) = (void(*)(void*)) regs[A2];
            void* myArg = (void*) regs[A3];


            *myHandle = KThread::createUserThread(myHandle, start_routine, myArg);
            int returnValue = 0;
            if (*myHandle == nullptr) {
                returnValue = -1;
            }
			regs[A0] = (uint64) returnValue;
            break;
        }


		case 0x12:
        {
            int returnValue = KThread::thread_exit();
			regs[A0] = (uint64) returnValue;
            break;
        }




		case 0x13:

        {
            KThread::thread_dispatch();
			regs[A0] = 0;
			break;
        }




		case 0x21:
        {
            sem_t* myHandle = (sem_t*) regs[A1];
            unsigned init = (unsigned)regs[A2];


            *myHandle = Semaphore::sem_open(myHandle, init);
            int returnValue = 0;
            if (*myHandle == nullptr) {
                returnValue = -1;
            }
			regs[A0] = (uint64) returnValue;
            break;
        }



		case 0x22:
        {
            sem_t myHandle = (sem_t) regs[A1];

            int returnValue = Semaphore::sem_close(myHandle);
			regs[A0] = (uint64) returnValue;
            break;
        }



		case 0x23:
        {
            sem_t id = (sem_t) regs[A1];

            int returnValue = Semaphore::sem_wait(id);
			regs[A0] = (uint64) returnValue;
            break;
        }



		case 0x24:
        {
            sem_t id = (sem_t) regs[A1];
            int returnValue = Semaphore::sem_signal(id);
			regs[A0] = (uint64) returnValue;
            break;
        }
		case 0x25:
		{
			sem_t id = (sem_t) regs[A1];
			unsigned n = (unsigned) regs[A2];

			int returnValue = Semaphore::sem_wait_n(id, n);
			regs[A0] = (uint64) returnValue;
			break;
		}
		case 0x26:
		{
			sem_t id = (sem_t) regs[A1];
			unsigned n = (unsigned) regs[A2];

			int returnValue = Semaphore::sem_signal_n(id, n);
			regs[A0] = (uint64) returnValue;
			break;
		}





		case 0x31:
        {
            time_t myDuration = (time_t) regs[A1];

            int returnValue = KThread::time_sleep(myDuration);
			regs[A0] = (uint64) returnValue;
            break;
        }


		case 0x41:
        {
			//__asm__ __volatile__("csrsi sstatus, 0x2" ::: "memory");

            char c = KConsole::getc();

			//__asm__ __volatile__("csrci sstatus, 0x2" ::: "memory");

			regs[A0] = (uint64) c;
            break;
        }


		case 0x42:
        {
            char myChar = (char) regs[A1];

            KConsole::putc(myChar);
			regs[A0] = 0;

            break;
        }
		default:
			regs[A0] = (uint64) -1;
			break;

	}










}

void RiscV::handleConsoleTrap() {
	int irq = plic_claim();

	if (irq == CONSOLE_IRQ) {
		char status = *(char*) CONSOLE_STATUS;

		while (status & CONSOLE_RX_STATUS_BIT) {
			char c = *(volatile char*)CONSOLE_RX_DATA;


			KConsole::inputBuffer->tryPut(c);

			status = *(volatile char*)CONSOLE_STATUS;
		}

		while (status & CONSOLE_TX_STATUS_BIT) {
			char c;

			if (!KConsole::outputBuffer->tryGet(c)) {
				break;
			}

			*(volatile char*)CONSOLE_TX_DATA = c;
			status = *(volatile char*)CONSOLE_STATUS;
		}
	}

	plic_complete(irq);

}
