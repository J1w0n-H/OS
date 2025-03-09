#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>

int main() {
    pid_t pid1, pid2;
    int status;
    
    // Create file for memory mapping
    int fd = open("mmapfile", O_RDWR | O_CREAT, 0666);
    ftruncate(fd, sizeof(int) * 2);
    
    // Create memory mapping
    int *numbers = mmap(NULL, sizeof(int) * 2, 
                       PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    pid1 = fork();
    
    if (pid1 == 0) {
        printf("First child: Enter two numbers: ");
        scanf("%d %d", &numbers[0], &numbers[1]);
        printf("First child: Numbers saved in mapped memory\n");
        munmap(numbers, sizeof(int) * 2);
        exit(0);
    }
    else {
        wait(&status);
        
        pid2 = fork();
        
        if (pid2 == 0) {
            printf("Second child: Calculating sum\n");
            printf("Sum: %d\n", numbers[0] + numbers[1]);
            munmap(numbers, sizeof(int) * 2);
            exit(0);
        }
        else {
            wait(&status);
            munmap(numbers, sizeof(int) * 2);
            close(fd);
            remove("mmapfile");
            printf("Parent: All done\n");
        }
    }
    return 0;
}

