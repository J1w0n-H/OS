#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
    pid_t pid1, pid2;
    int status;
    int num1, num2;
    int fd;
    
    // Setup FIFO
    unlink("sumfifo");
    if (mkfifo("sumfifo", 0666) < 0) {
        perror("mkfifo");
        exit(1);
    }
    
    pid1 = fork();   // Create first child
    
    if (pid1 == 0) {  // First child
        printf("First child: Enter two numbers: ");
        scanf("%d %d", &num1, &num2);
        
        // Write to FIFO
        fd = open("sumfifo", O_WRONLY | O_NONBLOCK);
        if (fd < 0) {
            perror("Child 1 - Cannot open FIFO");
            exit(1);
        }
        write(fd, &num1, sizeof(int));
        write(fd, &num2, sizeof(int));
        close(fd);
        printf("First child: Numbers sent\n");
        exit(0);
    }
    else {
        // Parent opens FIFO for reading
        fd = open("sumfifo", O_RDONLY | O_NONBLOCK);
        if (fd < 0) {
            perror("Parent - Cannot open FIFO");
            exit(1);
        }
        
        wait(&status);  // Wait for first child
        
        pid2 = fork();  // Create second child
        
        if (pid2 == 0) {  // Second child
            // Read from FIFO
            read(fd, &num1, sizeof(int));
            read(fd, &num2, sizeof(int));
            close(fd);
            
            printf("Second child: Calculating sum\n");
            printf("Sum of %d and %d is %d\n", num1, num2, num1 + num2);
            sleep(5);exit(0);
        }
        else {
            close(fd);
            wait(&status);  // Wait for second child
            unlink("sumfifo");  // Clean up FIFO
            printf("Parent: All done\n");
        }
    }
    return 0;
}
