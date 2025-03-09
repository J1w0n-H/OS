#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main() {
    pid_t pid1, pid2;
    int status;

    // Create shared memory
    key_t shm_key = ftok("shmfile", 65);
    int shmid = shmget(shm_key, sizeof(int) * 2, IPC_CREAT | 0666);
    int *shared_nums = (int*)shmat(shmid, NULL, 0);
    
    // Create semaphore
    key_t sem_key = ftok("semfile", 66);
    int semid = semget(sem_key, 1, IPC_CREAT | 0666);
    union semun arg;
    arg.val = 0;
    semctl(semid, 0, SETVAL, arg);
    
    struct sembuf wait_op = {0, -1, 0};
    struct sembuf signal_op = {0, 1, 0};
    
    pid1 = fork();
    
    if (pid1 == 0) {
        printf("First child: Enter two numbers: ");
        scanf("%d %d", &shared_nums[0], &shared_nums[1]);
        printf("First child: Numbers saved\n");
        
        // Signal that data is ready
        semop(semid, &signal_op, 1);
        shmdt(shared_nums);
        exit(0);
    }
    else {
        wait(&status);
        
        pid2 = fork();
        
        if (pid2 == 0) {
            // Wait for data to be ready
            semop(semid, &wait_op, 1);
            
            printf("Second child: Calculating sum\n");
            printf("Sum: %d\n", shared_nums[0] + shared_nums[1]);
            shmdt(shared_nums);
            exit(0);
        }
        else {
            wait(&status);
            shmdt(shared_nums);
            shmctl(shmid, IPC_RMID, NULL);
            semctl(semid, 0, IPC_RMID);
            printf("Parent: All done\n");
        }
    }
    return 0;
}
