#ifndef _ERRORCODE_H
#define _ERRORCODE_H

typedef int errorCode_t;

#define IS_ERROR(n) (n < SUCCESS)

#define SUCCESS    0
#define EBADPARM  -1
#define ENOTFOUND -2
#define EBADEXEC  -3
#define EMEMORY   -4
#define EFILESYS  -5

#endif