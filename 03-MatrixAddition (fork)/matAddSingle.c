#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>

#define NS_PER_SEC 1000000000

static inline uint64_t gettime_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * NS_PER_SEC + ts.tv_nsec;
}

int main(int argc, char* argv[])
{
    uint64_t start, end;
    int rows, cols;

    // Matrix 1
    FILE* input1 = fopen(argv[1], "r");
    fscanf(input1, "%d", &rows);
    fscanf(input1, "%d", &cols);
    int * matrix1 = (int *)malloc(rows*cols*sizeof(int));
    for(int i = 0; i < rows*cols; i++) {
        int value;
        fscanf(input1, "%d", &value);
        matrix1[i] = value;
    }
    fclose(input1);

    // Matrix 2
    FILE* input2 = fopen(argv[2], "r");
    fscanf(input2, "%d", &rows);
    fscanf(input2, "%d", &cols);

    int * matrix2 = (int *)malloc(rows*cols*sizeof(int));
    for(int i = 0; i < rows*cols; i++) {
        int value;
        fscanf(input2, "%d", &value);
        matrix2[i] = value;
    }
    fclose(input2);


    start = gettime_ns();
    int * result = (int *)malloc(rows*cols*sizeof(int));

    // Perform Single Process Matrix Addition
    for(int i= 0; i < rows*cols; i++){
        result[i] = matrix1[i] + matrix2[i];
    }

    end = gettime_ns();

    // Print result to console
    for (int i = 0; i < rows * cols; i++) {
        printf("%2d ", result[i]);
        if ((i + 1) % cols == 0) {
            printf("\n");
        }
    }


    printf("Addition took %" PRIu64 " nanoseconds\n", end - start);

    free(matrix1);
    free(matrix2);
    free(result);
}


