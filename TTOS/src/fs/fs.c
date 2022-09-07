#include <unistd.h>
#include <sys/syscall.h>

#include "errorCode.h"

errorCode_t fs_open(const char* path, int flags, int* fd) {
    if(fd == 0) {
        return EBADPARM;
    }
    int rc = syscall(SYS_open, path, flags);
    if(rc == -1) {
        return EFILESYS;
    }
    *fd = rc;
    return SUCCESS;
}

errorCode_t fs_write(int fd, const void *buf, int count, int* bytes_written) {
    int rc = syscall(SYS_write, fd, buf, count);
    if(rc == -1) {
        return EFILESYS;
    }
    if(bytes_written != 0) {
        *bytes_written = rc;
    }
    return SUCCESS;
}

errorCode_t fs_read(int fd, void *buf, int count, int* bytes_read) {
    int rc = syscall(SYS_read, fd, buf, count);
    if(rc == -1) {
        return EFILESYS;
    }
    if(bytes_read != 0) {
        *bytes_read = rc;
    }
    return SUCCESS;
}

errorCode_t fs_seek(int fd, int offset, int whence, int* distance) {
    int rc = syscall(SYS_lseek, fd, offset, whence);
    if(rc == -1) {
        return EFILESYS;
    }
    if(distance != 0) {
        *distance = rc;
    }
    return SUCCESS;
}

errorCode_t fs_close(int fd) {
    int rc = syscall(SYS_close, fd);
    if(rc == -1) {
        return EFILESYS;
    }
    return SUCCESS;
}