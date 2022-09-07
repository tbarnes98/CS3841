#include <stdarg.h>
#include <stdio.h>

#include "fs/fs.h"

long system_call(long n, ...) {
    long rc = 0;

    va_list ap;
    va_start(ap, n);

    if(n == 1) {
        int fd = va_arg(ap, int);
        const char* buf = va_arg(ap, const char*);
        int count = va_arg(ap, int);
        int* bytes_written = va_arg(ap, int*);
        rc = fs_write(fd, buf, count, bytes_written);
    }

    return rc;
}