#include <stdint.h>
#include <string.h>

#include "errorCode.h"
#include "elf.h"
#include "process.h"
#include "memory.h"

static errorCode_t allocate_process(int size, process* proc) {
    int aligned_size = PAGE_ALIGN(size);
    int rc = allocate_user_memory(aligned_size + STACK_SIZE, &(proc->exec_start));
    if(IS_ERROR(rc)) {
        return rc;
    }
    proc->stack_start = ((uint8_t*)(proc->exec_start) + aligned_size);
    proc->stack_end = ((uint8_t*)(proc->stack_start) + STACK_SIZE);
    proc->size = aligned_size + STACK_SIZE;
    return SUCCESS;
}

int create_process(const char* path, process* proc) {
    if(proc == 0) {
        return EBADPARM;
    }

    // Load the executable file
    elf_file file;
    int rc = load_elf(path, &file);
    if(IS_ERROR(rc)) {
        return rc;
    }

    // Allocate the process address space
    uint64_t proc_size = 0;
    rc = get_elf_proc_size(&file, &proc_size);
    if(IS_ERROR(rc)) {
        free_elf(&file);
        return rc;
    }
    memset(proc, 0x00, sizeof(process));
    rc = allocate_process(proc_size, proc);
    if(IS_ERROR(rc)) {
        free_elf(&file);
        return rc;
    }

    // Populate the process address space
    rc = populate_elf_proc(&file, proc->exec_start);
    if(IS_ERROR(rc)) {
        free_elf(&file);
        free_process(proc);
        return rc;
    }

    // Set the process entry point
    uint64_t offset_to_main = 0;
    rc = find_symbol(&file, "main", &offset_to_main);
    if(IS_ERROR(rc)) {
        free_elf(&file);
        free_process(proc);
        return rc;
    }
    proc->entry_point = (void*)((uint8_t*)proc->exec_start + offset_to_main);

    // Free the executable - no longer needed since process has been created
    free_elf(&file);
    return SUCCESS;
}

errorCode_t free_process(process* proc) {
    if(proc == 0) {
        return EBADPARM;
    }
    free_user_memory(proc->exec_start, proc->size);
    proc->exec_start = 0;
    proc->stack_start = 0;
    proc->stack_end = 0;
    proc->size = 0;
    return SUCCESS;
}