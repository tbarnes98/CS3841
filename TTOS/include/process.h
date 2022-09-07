#ifndef _PROCESS_H
#define _PROCESS_H

#define STACK_SIZE  8192

#include "elf.h"

typedef struct process {
    void* entry_point;
    void* exec_start;
    void* stack_start;
    void* stack_end;
    int size;
} process;

errorCode_t create_process(const char* file, process* proc);
errorCode_t free_process(process* proc);

#endif
