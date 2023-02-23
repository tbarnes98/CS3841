#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <pthread.h>
#include <stdatomic.h>

#define NS_PER_SEC 1000000000
#define OPS rows*cols
#define CORE 4

static int rows, cols;
int* matrix1;
int* matrix2;
int* result;



void *addition(void *arg) {
    int tid = *(int *)arg;
    if(tid < OPS) { 
        for(int j = tid*(OPS/CORE); j < (tid*(OPS/CORE) + (OPS/CORE)); j++) {
            result[j] = matrix1[j] + matrix2[j];
        }
    }
    free(arg);
    return NULL;
}



static inline uint64_t gettime_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * NS_PER_SEC + ts.tv_nsec;
}

int main(int argc, char* argv[]) {

    // Debug args
    //argv[1] = "matrices/matA";
    //argv[2] = "matrices/matB";

    uint64_t start, end;

    // Matrix 1
    FILE* input1 = fopen(argv[1], "r");
    fscanf(input1, "%d", &rows);
    fscanf(input1, "%d", &cols);
    matrix1 = (int *)malloc(rows*cols*sizeof(int));
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

    matrix2 = (int *)malloc(rows*cols*sizeof(int));
    for(int i = 0; i < rows*cols; i++) {
        int value;
        fscanf(input2, "%d", &value);
        matrix2[i] = value;
    }
    fclose(input2);

    result = (int *)malloc(rows*cols*sizeof(int));

    // Threading functionality

    start = gettime_ns();

    if(OPS >= CORE) {
        pthread_t threads[CORE];
        int remainder = (OPS) % CORE;
        // Create threads
        for (int i = 0; i < CORE; i++) {
            int* a = malloc(sizeof(int));
            *a = i;
            if (pthread_create(&threads[i], NULL, addition, a)) {
                perror("Error creating thread\n");
                return 1;
            }
        }
        // Wait for threads
        for (int i = 0; i < CORE; i++) {
            if (pthread_join(threads[i], NULL)) {
                perror("Error joining thread\n");
                return 1;
            }
        }
        // Add remaining elements
        if(remainder != 0) {
            for (int i = (OPS); i < (OPS)+remainder; i++) {
                result[i] = matrix1[i] + matrix2[i];
            }
        }
    } else {
        for (int i = OPS; i < OPS; i++) {
            result[i] = matrix1[i] + matrix2[i];
        }
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