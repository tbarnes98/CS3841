#ifndef _FS_H
#define _FS_H

#include "errorCode.h"

/* fs_open flags */
#define O_RDONLY	00000000
#define O_WRONLY	00000001
#define O_RDWR		00000002

/* fs_seek whence */
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

errorCode_t fs_open(const char* path, int flags, int* fd);
errorCode_t fs_write(int fd, const void *buf, int count, int* bytes_written);
errorCode_t fs_read(int fd, void *buf, int count, int* bytes_read);
errorCode_t fs_seek(int fd, int offset, int whence, int* distance);
errorCode_t fs_close(int fd);

#endif