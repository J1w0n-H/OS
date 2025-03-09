#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();
    
    if (pid < 0) {
        printf("Fork failed\n");
        return 1;
    }
    else if (pid == 0) {
        printf("I am child process (PID: %d)\n", getpid());
    }
    else {
        printf("I am parent process (PID: %d)\n", getpid());
    }
    return 0;
}
