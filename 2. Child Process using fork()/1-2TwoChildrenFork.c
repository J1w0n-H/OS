#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t pid1, pid2;
    
    pid1 = fork();  // First fork
    
    if (pid1 < 0) {
        printf("First fork failed\n");
        return 1;
    }
    else if (pid1 == 0) {
        printf("I am first child (PID: %d)\n", getpid());
    }
    else {
        pid2 = fork();  // Second fork
        
        if (pid2 < 0) {
            printf("Second fork failed\n");
            return 1;
        }
        else if (pid2 == 0) {
            printf("I am second child (PID: %d)\n", getpid());
        }
        else {
            printf("I am parent (PID: %d)\n", getpid());
        }
    }
    return 0;
}
