#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h> 

int main() {
    pid_t pid1, pid2;
    int status;
    int num1=0, num2=0;
    
    pid1 = fork();
    
    if (pid1 < 0) {
        printf("First fork failed\n");
        return 1;
    }
    else if (pid1 == 0) {
        printf("First child: Enter two numbers: ");
        scanf("%d %d", &num1, &num2);
        printf("First child: Numbers entered\n");
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
            printf("Second child: Trying to calculate sum\n");
            // This won't work! Can't access nums from first child
            printf("Sum: %d\n", num1 + num2);
            exit(0);
        }
        else {
            wait(&status);
            printf("Parent: All done\n");
        }
    }
    return 0;
}
