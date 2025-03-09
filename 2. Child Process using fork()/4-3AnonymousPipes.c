#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h> 

int main() {
    pid_t pid1, pid2;
    int status;
    int pipe_fd[2];
    int nums[2];
    
    // Create pipe before forking
    if (pipe(pipe_fd) == -1) {
        printf("Pipe failed\n");
        return 1;
    }
    
    pid1 = fork();
    
    if (pid1 < 0) {
        printf("First fork failed\n");
        return 1;
    }
    else if (pid1 == 0) {
        close(pipe_fd[0]);  // Close read end
        
        printf("First child: Enter two numbers: ");
        scanf("%d %d", &nums[0], &nums[1]);
        
        write(pipe_fd[1], nums, sizeof(nums));
        close(pipe_fd[1]);
        exit(0);
    }
    else {
        wait(&status);
        
        pid2 = fork();
        
        if (pid2 < 0) {
            printf("Second fork failed\n");
            return 1;
        }
        else if (pid2 == 0) {
            close(pipe_fd[1]);  // Close write end
            
            int received[2];
            read(pipe_fd[0], received, sizeof(received));
            printf("Sum: %d\n", received[0] + received[1]);
            
            close(pipe_fd[0]);
            exit(0);
        }
        else {
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            wait(&status);
            printf("Parent: All done\n");
        }
    }
    return 0;
}
