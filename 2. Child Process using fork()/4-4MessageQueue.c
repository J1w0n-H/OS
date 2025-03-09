#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>

struct msg_buffer {
    long msg_type;
    int numbers[2];
} message;

int main() {
    pid_t pid1, pid2;
    int status;

    // Create message queue
    key_t key = ftok("msgfile", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    
    pid1 = fork();
    
    if (pid1 == 0) {
        printf("First child: Enter two numbers: ");
        scanf("%d %d", &message.numbers[0], &message.numbers[1]);
        message.msg_type = 1;
        msgsnd(msgid, &message, sizeof(message.numbers), 0);
        printf("First child: Numbers sent to queue\n");
        exit(0);
    }
    else {
        wait(&status);
        
        pid2 = fork();
        
        if (pid2 == 0) {
            msgrcv(msgid, &message, sizeof(message.numbers), 1, 0);
            printf("Second child: Calculating sum\n");
            printf("Sum: %d\n", message.numbers[0] + message.numbers[1]);
            exit(0);
        }
        else {
            wait(&status);
            msgctl(msgid, IPC_RMID, NULL);
            printf("Parent: All done\n");
        }
    }
    return 0;
}
