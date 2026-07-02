#include "../h/MemoryAllocator.hpp"

static const int MAX_BLOCK_DESCRIPTORS = 4096;

static Block blockPool[MAX_BLOCK_DESCRIPTORS];
static bool blockPoolUsed[MAX_BLOCK_DESCRIPTORS];

static Block* allocBlockDescriptor() {
    for (int i = 0; i < MAX_BLOCK_DESCRIPTORS; i++) {
        if (!blockPoolUsed[i]) {
            blockPoolUsed[i] = true;
            blockPool[i].startAddress = 0;
            blockPool[i].endAddress = 0;
            blockPool[i].prev = nullptr;
            blockPool[i].next = nullptr;
            return &blockPool[i];
        }
    }

    return nullptr;
}

static void freeBlockDescriptor(Block* b) {
    if (b == nullptr) return;

    for (int i = 0; i < MAX_BLOCK_DESCRIPTORS; i++) {
        if (&blockPool[i] == b) {
            blockPoolUsed[i] = false;
            b->startAddress = 0;
            b->endAddress = 0;
            b->prev = nullptr;
            b->next = nullptr;
            return;
        }
    }
}

Block MemoryAllocator::headOfFreeListData;
Block MemoryAllocator::headOfUsedListData;

Block* MemoryAllocator::headOfFreeList = &headOfFreeListData;
Block* MemoryAllocator::headOfUsedList = nullptr;





void MemoryAllocator::init() {

    headOfFreeList = &headOfFreeListData;

    MemoryAllocator::headOfFreeList->startAddress = (uint64) HEAP_START_ADDR;
    MemoryAllocator::headOfFreeList->endAddress =  (uint64) HEAP_END_ADDR;
    MemoryAllocator::headOfFreeList->prev = nullptr;
    MemoryAllocator::headOfFreeList->next = nullptr;

}

void* MemoryAllocator::mem_alloc(size_t size) {
    if (size == 0) return nullptr;

    Block* curr = headOfFreeList;

    while (curr != nullptr) {
        size_t currSize = curr->endAddress - curr->startAddress + 1;

        if (currSize >= size) break;

        curr = curr->next;
    }

    if (curr == nullptr) return nullptr;

    uint64 allocStart = curr->startAddress;
    uint64 allocEnd = curr->startAddress + size - 1;
    Block* usedBlock = allocBlockDescriptor();
    if (usedBlock == nullptr) {
        return nullptr;
    }

    usedBlock->startAddress = allocStart;
    usedBlock->endAddress = allocEnd;
    usedBlock->prev = nullptr;
    usedBlock->next = nullptr;

    size_t currSize = curr->endAddress - curr->startAddress + 1;

    if (currSize > size) {
        //shrink free block from the front
        curr->startAddress = allocEnd + 1;
    }
    else {
        //remove whole free block
        if (curr->prev != nullptr) {
            curr->prev->next = curr->next;
        }
        else {
            headOfFreeList = curr->next;
        }

        if (curr->next != nullptr) {
            curr->next->prev = curr->prev;
        }
        curr->prev = nullptr;
        curr->next = nullptr;
    }



    Block* next = headOfUsedList;
    Block* prev = nullptr;

    while (next != nullptr && next->startAddress < usedBlock->startAddress) {
        prev = next;
        next = next->next;
    }

    usedBlock->prev = prev;
    usedBlock->next = next;

    if (prev != nullptr) {
        prev->next = usedBlock;
    }
    else {
        headOfUsedList = usedBlock;
    }

    if (next != nullptr) {
        next->prev = usedBlock;
    }

    return (void*) usedBlock->startAddress;
}


int MemoryAllocator::mem_free(void* ptr) {
    if (ptr == nullptr) return -1;

    Block* curr = headOfUsedList;

    while (curr != nullptr && (void*)curr->startAddress != ptr) {
        if ((void*)curr->startAddress > ptr) {
            return -1;
        }

        curr = curr->next;


    }

    if (curr == nullptr) return -1;

    //remove from used list
    if (curr->prev != nullptr) {
        curr->prev->next = curr->next;
    }
    else {
        headOfUsedList = curr->next;
    }

    if (curr->next != nullptr) {
        curr->next->prev = curr->prev;
    }

    curr->prev = nullptr;
    curr->next = nullptr;

    //insert into free list sorted by startAddress
    Block* next = headOfFreeList;
    Block* prev = nullptr;

    while (next != nullptr && next->startAddress < curr->startAddress) {
        prev = next;
        next = next->next;
    }

    curr->prev = prev;
    curr->next = next;

    if (prev != nullptr) {
        prev->next = curr;
    }
    else {
        headOfFreeList = curr;
    }
    if (next != nullptr) {
        next->prev = curr;
    }

    //merge with previous
    if (curr->prev != nullptr && curr->prev->endAddress + 1 == curr->startAddress) {
        Block* previous = curr->prev;

        previous->endAddress = curr->endAddress;
        previous->next = curr->next;

        if (curr->next != nullptr) {
            curr->next->prev = previous;
        }

        freeBlockDescriptor(curr);
        curr = previous;
    }
    //merge with next
    if (curr->next != nullptr && curr->endAddress + 1 == curr->next->startAddress) {
        Block* nextBlock = curr->next;

        curr->endAddress = nextBlock->endAddress;
        curr->next = nextBlock->next;

        if (nextBlock->next != nullptr) {
            nextBlock->next->prev = curr;
        }
        freeBlockDescriptor(nextBlock);

    }

    return 0;
}








size_t MemoryAllocator::mem_get_free_space() {

    Block* curr = headOfFreeList;

    size_t totalFreeSpace = 0;

    while (curr != nullptr) {
        totalFreeSpace += (char*) curr->endAddress - (char*) curr->startAddress + 1;
        curr = curr->next;
    }

    return totalFreeSpace;


}

size_t MemoryAllocator::mem_get_largest_free_block() {

    size_t largestFreeBlockSize = 0;

    Block* curr = headOfFreeList;

    while (curr != nullptr) {
        size_t currBlockSize = curr->endAddress - curr->startAddress + 1;
        if (currBlockSize > largestFreeBlockSize) {
            largestFreeBlockSize = currBlockSize;
        }
        curr = curr->next;
    }

    return largestFreeBlockSize;
}

