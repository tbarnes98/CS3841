#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#include "errorCode.h"
#include "memory.h"

int allocate_user_memory(int size, void** addr) {
    if(addr == 0) {
        return EBADPARM;
    }

    uint64_t aligned_size = PAGE_ALIGN(size);
    *addr = mmap(NULL, aligned_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(*addr == MAP_FAILED) {
        *addr = 0;
        return EMEMORY;
    }

    return SUCCESS;
}

int free_user_memory(void* addr, int size) {
    if(addr == 0) {
        return EBADPARM;
    }
    int rc = munmap(addr, size);
    if(rc == -1) {
        return EMEMORY;
    }
    return SUCCESS;
}

int get_kernel_memory(int size, void** addr) {
    if(addr == 0) {
        return EBADPARM;
    }
    *addr = malloc(size);
    if(*addr == 0) {
        return EMEMORY;
    }
    return SUCCESS;
}

int free_kernel_memory(void* addr, int size) {
    if(addr == 0) {
        return EBADPARM;
    }
    free(addr);
    return SUCCESS;
}