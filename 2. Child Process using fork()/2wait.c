#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid1, pid2;
    int status;
    
    pid1 = fork();
    
    if (pid1 < 0) {
        printf("First fork failed\n");
        return 1;
    }
    else if (pid1 == 0) {
        printf("First child starting (PID: %d)\n", getpid());
        sleep(2);  // Simulate work
        printf("First child finishing\n");
    }
    else {
        wait(&status);  // Wait for first child
        printf("Parent: First child finished\n");
        
        pid2 = fork();
        
        if (pid2 < 0) {
            printf("Second fork failed\n");
            return 1;
        }
        else if (pid2 == 0) {
            printf("Second child starting (PID: %d)\n", getpid());
            sleep(1);  // Simulate work
            printf("Second child finishing\n");
        }
        else {
            wait(&status);  // Wait for second child
            printf("Parent: All children finished\n");
        }
    }
    return 0;
}
