#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4

int *matrix1;
int *matrix2;
int *result;
int num_rows;
int num_cols;

void *matrix_addition(void *arg) {
    int thread_id = *(int *)arg;
    int i, j;
    for (i = thread_id; i < num_rows; i += NUM_THREADS) {
        for (j = 0; j < num_cols; j++) {
            int index = i * num_cols + j;
            result[index] = matrix1[index] + matrix2[index];
        }
    }
    pthread_exit(NULL);
}

int main() {
    int i, j;
    num_rows = 3;
    num_cols = 3;

    // Allocate memory for matrices
    matrix1 = malloc(num_rows * num_cols * sizeof(int));
    matrix2 = malloc(num_rows * num_cols * sizeof(int));
    result = malloc(num_rows * num_cols * sizeof(int));

    // Initialize matrices
    for (i = 0; i < num_rows; i++) {
        for (j = 0; j < num_cols; j++) {
            int index = i * num_cols + j;
            matrix1[index] = rand() % 10;
            matrix2[index] = rand() % 10;
        }
    }

    // Create threads
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    for (i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, matrix_addition, &thread_ids[i]);
    }

    // Join threads
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print result matrix
    printf("Result matrix:\n");
    for (i = 0; i < num_rows; i++) {
        for (j = 0; j < num_cols; j++) {
            printf("%d ", result[i * num_cols + j]);
        }
        printf("\n");
    }

    // Free memory
    free(matrix1);
    free(matrix2);
    free(result);

    return 0;
}
