#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h> 

int main() {
    pid_t pid1, pid2;
    int status;
    int num1, num2;
    
    pid1 = fork();
    
    if (pid1 < 0) {
        printf("First fork failed\n");
        return 1;
    }
    else if (pid1 == 0) {
        printf("First child: Enter two numbers: ");
        scanf("%d %d", &num1, &num2);
        
        FILE *fp = fopen("numbers.txt", "w");
        fprintf(fp, "%d %d", num1, num2);
        fclose(fp);
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
            FILE *fp = fopen("numbers.txt", "r");
            fscanf(fp, "%d %d", &num1, &num2);
            fclose(fp);
            
            printf("Sum: %d\n", num1 + num2);
            exit(0);
        }
        else {
            wait(&status);
            printf("Parent: All done\n");
            remove("numbers.txt");  // Clean up
        }
    }
    return 0;
}
