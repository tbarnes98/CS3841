#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int A[MAX][MAX], B[MAX][MAX], C[MAX][MAX];
    int i, j, k, sum = 0;
    int shmid;
    int *shm;

    // Allocate shared memory
    shmid = shmget(IPC_PRIVATE, MAX * MAX * sizeof(int), 0666 | IPC_CREAT);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    // Attach shared memory
    shm = shmat(shmid, NULL, 0);
    if (shm == (int *)-1) {
        perror("shmat");
        exit(1);
    }

    // Initialize matrices A and B
    for (i = 0; i < MAX; i++) {
        for (j = 0; j < MAX; j++) {
            A[i][j] = i + j;
            B[i][j] = i - j;
        }
    }

    // Fork a child process
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    }
    else if (pid == 0) { // Child process
        for (i = 0; i < MAX; i++) {
            for (j = 0; j < MAX; j++) {
                sum = 0;
                for (k = 0; k < MAX; k++) {
                    sum += A[i][k] * B[k][j];
                }
                *(shm + i * MAX + j) = sum;
            }
        }
        exit(0);
    }
    else { // Parent process
        wait(NULL);

        // Retrieve result from shared memory
        for (i = 0; i < MAX; i++) {
            for (j = 0; j < MAX; j++) {
                C[i][j] = *(shm + i * MAX + j);
            }
        }

        // Print result matrix C
        printf("Matrix C:\n");
        for (i = 0; i < MAX; i++) {
            for (j = 0; j < MAX; j++) {
                printf("%d ", C[i][j]);
            }
            printf("\n");
        }

        // Detach and remove shared memory
        shmdt(shm);
        shmctl(shmid, IPC_RMID, NULL);
        munmap(shm);
    }

    return 0;
}