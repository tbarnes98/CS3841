#ifndef _MEMORY_H
#define _MEMORY_H

#include "errorCode.h"

#define PAGE_SIZE   4096
#define PAGE_ALIGN(n) (n + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1)

errorCode_t allocate_user_memory(int size, void** addr);
errorCode_t free_user_memory(void* addr, int size);

errorCode_t get_kernel_memory(int size, void** addr);
errorCode_t free_kernel_memory(void* addr, int size);

#endif