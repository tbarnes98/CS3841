#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "fs/fs.h"
#include "syscall.h"
#include "elf.h"
#include "process.h"
#include "memory.h"

static int validate_magic(uint8_t e_ident[EI_NIDENT]) {
    // ELF Magic must match
    if(e_ident[0] != ELFMAG0 || e_ident[1] != ELFMAG1 || e_ident[2] != ELFMAG2 || e_ident[3] != ELFMAG3) {
        return -1;
    }
    // Currently, only support 64 bit executables
    if(e_ident[4] != ELFCLASS64) {
        return -1;
    }
    // Currently, only support LSB byte order executables
    if(e_ident[5] != ELFDATA2LSB) {
        return -1;
    }
    // Currently, only support ELF current version
    if(e_ident[6] != EV_CURRENT) {
        return -1;
    }
    return 0;
}

static errorCode_t get_file_data(const char* path, elf_file* file) {
    int rc;

    // Open the file
    int fd = 0;
    rc = fs_open(path, O_RDONLY, &fd);
    if(IS_ERROR(rc)) {
        return rc;
    }

    // Seek to the beginning and then the end
    //   to get the file size
    fs_seek(fd, 0, SEEK_SET, 0);
    rc = fs_seek(fd, 0, SEEK_END, &file->size);
    if(IS_ERROR(file->size) || file->size == 0) {
        return EBADEXEC;
    }

    rc = get_kernel_memory(file->size, &file->contents);
    if(IS_ERROR(rc)) {
        return rc;
    }

    // Seek to the beginning and read the file data
    fs_seek(fd, 0, SEEK_SET, 0);
    int bytes_read = 0;
    rc = fs_read(fd, file->contents, file->size, &bytes_read);

    // Check error if error use error code if one was returned
    if(IS_ERROR(rc)) {
        free_kernel_memory(file->contents, file->size);
        fs_close(fd);
        return rc;
    }

    // Check error ELIBBAD if data read != size
    if(bytes_read != file->size) {
        free_kernel_memory(file->contents, file->size);
        fs_close(fd);
        return EBADEXEC;
    }

    // Close the file and return
    fs_close(fd);
    return 0;
}

static Elf64_Phdr* get_program_header(elf_file* file, int index) {
    return (Elf64_Phdr*)((char*)file->phdr + index * file->ehdr->e_phentsize);
}

static Elf64_Shdr* get_section_header(elf_file* file, int index) {
    return (Elf64_Shdr*)((char*)file->shdr + index * file->ehdr->e_shentsize);
}

static void* find_external_symbol(elf_file* file, Elf64_Shdr *symtbl_shdr, Elf64_Word st_name) {

    if(symtbl_shdr->sh_link == 0) {
        return 0;
    }

    Elf64_Shdr* strtbl_hdr = get_section_header(file, symtbl_shdr->sh_link);
    char* strtbl = (char*)((uint8_t*)file->contents + strtbl_hdr->sh_offset);
    char* symbol_name = &(strtbl[st_name]);

    if(strncmp(symbol_name, "system_call", 7) == 0) {
        return system_call;
    }
    return 0;
}

static errorCode_t relocate_symbols(elf_file* file, void* tgt_addr) {

    for(int i = 0; i < file->ehdr->e_shnum; i++) {

        Elf64_Shdr* shdr = get_section_header(file, i);

        if(shdr->sh_type == SHT_REL || shdr->sh_type == SHT_RELA) {

            // Retrieve the sections headers for the relocation section, linked symbol table, and target section
            Elf64_Shdr* rel_shdr = shdr;
            Elf64_Shdr *symtbl_shdr = get_section_header(file, rel_shdr->sh_link);
            Elf64_Shdr *tgt_shdr = get_section_header(file, rel_shdr->sh_info);

            // Retrieve the linked symbol table
            Elf64_Sym* symtbl = (Elf64_Sym*)((uint8_t*)file->contents + symtbl_shdr->sh_offset);

            // Loop through each relocation entry and update the target
            int entry_count = rel_shdr->sh_size / rel_shdr->sh_entsize;
            for (int j = 0; j < entry_count; j++) {

                Elf64_Rela* relhdr = (Elf64_Rela*)((uint8_t*)file->contents + rel_shdr->sh_offset + j*rel_shdr->sh_entsize);
                Elf64_Xword symbol = ELF64_R_SYM(relhdr->r_info);
                Elf64_Xword type = ELF64_R_TYPE(relhdr->r_info);

                // Symbol address
                //  For relocatable files - start of the exec + offset to the section containing the symbol + offset to the symbol in the section (aka symbol value)
                //  For all other files - start of the exec + symbol value (aka virtual address)
                //  For undefined symbols - search for the symbol externally
                void* symbol_address = 0;
                if(file->ehdr->e_type == ET_REL) {
                    symbol_address = (void*)((uint8_t*)tgt_addr + file->shdr[symtbl[symbol].st_shndx].sh_offset + symtbl[symbol].st_value);
                } else if(symtbl[symbol].st_shndx == STN_UNDEF) {
                    symbol_address = find_external_symbol(file, symtbl_shdr, symtbl[symbol].st_name);
                } else {
                    symbol_address = (void*)((uint8_t*)tgt_addr + symtbl[symbol].st_value);
                }
                if(symbol_address == 0) {
                    return -1;
                }

                // Location in section to patch
                //  For relocatable files - start of exec + offset to patch section + offset in the section
                //  For all other files - start of the exec + relocation offset value (aka virtual address)
                void* patch_location = file->ehdr->e_type == ET_REL ?
                        (void*)((uint8_t*)tgt_addr + tgt_shdr->sh_offset + relhdr->r_offset) :
                        (void*)((uint8_t*)tgt_addr + relhdr->r_offset);

                // retrieve the addend, only applies to RELA entries
                Elf64_Sxword addend = file->shdr[i].sh_type == SHT_RELA ? relhdr->r_addend : 0;

                // https://refspecs.linuxbase.org/elf/x86_64-abi-0.98.pdf page 69
                switch(type) {
                    case R_X86_64_PC8:
                        *(uint8_t*)patch_location = (uint8_t*)symbol_address + addend - (uint8_t*)patch_location;
                        break;
                    case R_X86_64_PC16:
                        *(uint16_t*)patch_location = (uint8_t*)symbol_address + addend - (uint8_t*)patch_location;
                    case R_X86_64_PC32:
                    case R_X86_64_PLT32:
                        *(uint32_t*)patch_location = (uint8_t*)symbol_address + addend - (uint8_t*)patch_location;
                        break;
                    case R_X86_64_PC64:
                        *(uint64_t*)patch_location = (uint8_t*)symbol_address + addend - (uint8_t*)patch_location;
                        break;
                    case R_X86_64_GLOB_DAT:
                    case R_X86_64_JUMP_SLOT:
                        *(uint64_t*)patch_location = (uint64_t)symbol_address;
                        break;
                }

//                printf("%lX %lX %016lX %016lX\n", symbol, type, (uint64_t)symbol_address, (uint64_t)patch_location);
            }
        }
    }
    return SUCCESS;
}

errorCode_t load_elf(const char* path, elf_file* file) {
    if(file == 0 || path == 0) {
        return EBADPARM;
    }

    // Initialize the file and retrieve the file data
    memset(file, 0x00, sizeof(elf_file));
    int rc = get_file_data(path, file);
    if(IS_ERROR(rc)) {
        return rc;
    }

    // Validate the file magic value
    if(validate_magic(file->contents) != 0) {
        free_elf(file);
        return EBADEXEC;
    }

    file->ehdr = (Elf64_Ehdr*)file->contents;

    /* Only support dynamic linked files */
    if(file->ehdr->e_type != ET_DYN) {
        free_elf(file);
        return EBADEXEC;
    }

    if(file->ehdr->e_phoff == 0) {
        free_elf(file);
        return EBADEXEC;
    }
    file->phdr = (Elf64_Phdr*)((uint8_t*)file->contents + file->ehdr->e_phoff);

    if(file->ehdr->e_shoff == 0) {;
        free_elf(file);
        return EBADEXEC;
    }
    file->shdr = (Elf64_Shdr*)((uint8_t*)file->contents + file->ehdr->e_shoff);

    return SUCCESS;
}

errorCode_t get_elf_proc_size(elf_file* file, uint64_t* size) {

    if(file == 0 || size == 0) {
        return EBADPARM;
    }

    // Determine the size to allocate for the process
    //  Determined by the highest address of any memory segment in the program header
    Elf64_Phdr* highest_header = 0;
    for(int i = 0; i < file->ehdr->e_phnum; i++) {
        Elf64_Phdr *phdr = get_program_header(file, i);
        if (phdr->p_type == PT_LOAD) {
            if (highest_header == 0 || highest_header->p_vaddr < phdr->p_vaddr) {
                highest_header = phdr;
            }
        }
    }

    *size = (uint64_t)highest_header->p_vaddr + (uint64_t)highest_header->p_memsz;
    return SUCCESS;
}

errorCode_t populate_elf_proc(elf_file* file, void* tgt_addr) {

    if(file == 0 || tgt_addr == 0) {
        return EBADPARM;
    }

    // Copy program sections into the process
    for(int i = 0; i < file->ehdr->e_shnum; i++) {
        Elf64_Shdr* shdr = get_section_header(file, i);
        if(shdr && (shdr->sh_flags & SHF_ALLOC) && shdr->sh_addr > 0) {
            memcpy(((uint8_t*)tgt_addr) + shdr->sh_addr, ((uint8_t*)file->contents) + shdr->sh_offset, shdr->sh_size);
        }
    }

    // Relocate symbols
    return relocate_symbols(file, tgt_addr);
}

errorCode_t find_symbol(elf_file* file, const char* name, uint64_t* offset) {

    if(offset == 0) {
        return EBADPARM;
    }

    // Search for a symbol
    //  Iterate through each section
    for(int i = 0; i < file->ehdr->e_shnum; i++) {

        // Find a section that represents a symbol table
        Elf64_Shdr* shdr = get_section_header(file, i);
        if(shdr->sh_type == SHT_SYMTAB) {

            // Retrieve the corresponding string table
            Elf64_Shdr* strtbl_hdr = get_section_header(file, shdr->sh_link);
            char* strtbl = (char*)((uint8_t*)file->contents + strtbl_hdr->sh_offset);

            // Iterate through each entry searching for the symbol string
            int entry_count = shdr->sh_size / shdr->sh_entsize;
            for(int j = 0; j < entry_count; j++) {
                Elf64_Sym* symtbl = (Elf64_Sym*)((uint8_t*)file->contents + shdr->sh_offset + j*shdr->sh_entsize);
                char* symbol_name = &(strtbl[symtbl->st_name]);

                // Check symbol name and return offset
                //  For relocatable files - offset to symbol table + offset to the symbol (aka symbol value)
                //  For all other files - symbol value (aka virtual address)
                if(strcmp(symbol_name, name) == 0) {
                    if(file->ehdr->e_type == ET_REL) {
                        *offset = file->shdr[symtbl->st_shndx].sh_offset + symtbl->st_value;
                    } else {
                        *offset = symtbl->st_value;
                    }
                    return SUCCESS;
                }
            }
        }
    }

    return ENOTFOUND;
}

errorCode_t free_elf(elf_file* file) {
    free_kernel_memory(file->contents, file->size);
    memset(file, 0x00, sizeof(elf_file));
    return SUCCESS;
}

