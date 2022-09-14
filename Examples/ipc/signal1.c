/*
 * signal1.c - Shows how to set up a signal handler
 *            for the interrupt (CTRL+C) signal
 */

#include <sys/types.h> // needed for pid_t
#include <unistd.h>    // needed for getpid, sleep system calls
#include <signal.h>    // needed for signal system call
#include <stdio.h>     // needed for printf

// Signal handler
//    Prints the PID of the process and the received signal
void signal_handler(int sig)
{
    pid_t pid = getpid();
    printf("Process %d received signal %d\n", pid, sig);
}

int main()
{
    // Set a signal handler for the interrupt signal
    signal(SIGINT, signal_handler);

    // Loop forever and sleep
    while(1) {
        printf("sleeping...\n");
        sleep(1);
    }
}
