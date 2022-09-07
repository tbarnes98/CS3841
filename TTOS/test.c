//#include <stdio.h>

long system_call(long n, ...);

int j = 3;
int i = 10;

int foo3(int b);

int foo2(int b) {
    return foo3(b) + 2;
}

int foo1(int a) {
    return foo2(a) + 2 + i;
}

int main(int argc, char* argv[]) {
//    printf("hi\n");
//    j = 10;
    system_call(1, 1, "abc\n", 4);
    int result = foo1(5 + j);
    system_call(1, 1, result, 4);
    system_call(1, 1, "\n", 1);
    return foo1(5 + j);
//    return foo1(5 + j);
}