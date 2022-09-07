#include <stdio.h>

#include "process.h"

int main(int argc, char* argv[]) {
    process out;
    printf("%d\n", create_process("test", &out));

    int (*main_exec)() = out.entry_point;
    int ret_val = main_exec();

    printf("%d\n", ret_val);

    free_process(&out);
    return 0;
}