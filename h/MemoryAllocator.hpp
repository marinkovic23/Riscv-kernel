#ifndef memory_allocator
#define memory_allocator

#include "../lib/hw.h"

typedef struct Block {
    size_t startAddress;
    size_t endAddress;
    Block* next;
    Block* prev;

}Block;


class MemoryAllocator {

public:

    static void init();

    static size_t mem_get_free_space();


protected:


private:

	friend class RiscV; //only through a system call can we access these functions
    friend class Semaphore;//semaphore calls the allocator sometimes
    friend class KThread;//thread also calls the allocator
    friend class Buffer;

	//to make sure we cannot instantiate this class, nobody needs it
    MemoryAllocator(const MemoryAllocator&) = delete;
    MemoryAllocator& operator=(const MemoryAllocator&) = delete;

    static void* mem_alloc (size_t size);

    static int mem_free (void*);



    static size_t mem_get_largest_free_block();

    static Block* headOfFreeList;
    static Block* headOfUsedList;

    static Block headOfFreeListData;
    static Block headOfUsedListData;



    //nobody needs to use this
    MemoryAllocator() {};



};






#endif
