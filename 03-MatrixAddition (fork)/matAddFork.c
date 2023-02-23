#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NS_PER_SEC 1000000000

static inline uint64_t gettime_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * NS_PER_SEC + ts.tv_nsec;
}

int main(int argc, char* argv[]) {

    // Debug args
    //argv[1] = "matrices/matC";
    //argv[2] = "matrices/matD";

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

    // Create shared memory object for result
    int shm_fd = shm_open("MATRIX", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error creating shared memory object");
        exit(1);
    }

    // Resize shared memory object to fit result
    if (ftruncate(shm_fd, rows*cols*sizeof(int))) {
        perror("Error resizing shared memory object");
        exit(1);
    }

    // Map shared memory object to C
    int *result = mmap(NULL, rows*cols*sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (result == MAP_FAILED) {
        perror("Error mapping shared memory object to C");
        exit(1);
    }

    // Allocate Memory for pids
    int* pids = (int*)malloc(rows*sizeof(pid_t));


    // Perform Forked Process Matrix Addition
    start = gettime_ns();
    for (int i = 0; i < rows; i++) {
        pids[i] = fork();
        if(pids[i] == -1) { 
            // Fork error
            perror("Error with creating process\n");
            return 1;
        }
        if(pids[i] == 0) { 
            // Child process
            for(int j = 0; j < cols; j++) {
                result[i*cols+j] = matrix1[i*cols+j] + matrix2[i*cols+j];
            }
            //printf("%d",pids[i]);
            return 1;
        }
    }
    end = gettime_ns();
    for (int i = 0; i < rows; i++) {
        waitpid(pids[i], NULL, 0);
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

    // Unmap and unlink shared memory object
    munmap(result, rows*cols*sizeof(int));
    close(shm_fd);
    shm_unlink("MATRIX");    
}