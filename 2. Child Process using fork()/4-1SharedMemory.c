#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>

int main() {
    pid_t pid1, pid2;
    int status;

    // Create shared memory
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(int) * 2, IPC_CREAT | 0666);
    int *shared_nums = (int*)shmat(shmid, NULL, 0);
    
    pid1 = fork();
    
    if (pid1 == 0) {
        printf("First child: Enter two numbers: ");
        scanf("%d %d", &shared_nums[0], &shared_nums[1]);
        printf("First child: Numbers saved in shared memory\n");
        shmdt(shared_nums);
        exit(0);
    }
    else {
        wait(&status);
        
        pid2 = fork();
        
        if (pid2 == 0) {
            printf("Second child: Calculating sum\n");
            int sum = shared_nums[0] + shared_nums[1];
            printf("Sum: %d\n", sum);
            shmdt(shared_nums);
	    sleep(5);
            exit(0);
        }
        else {
            wait(&status);
            shmdt(shared_nums);
            shmctl(shmid, IPC_RMID, NULL);
            printf("Parent: All done\n");
        }
    }
    return 0;
}
