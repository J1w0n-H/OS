#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/sum_socket"

int main() {
    pid_t pid1, pid2;
    int status;
    
    // Create socket
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);
    
    unlink(SOCKET_PATH);
    bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    listen(sock, 1);
    
    pid1 = fork();
    
    if (pid1 == 0) {
        int numbers[2];
        printf("First child: Enter two numbers: ");
        scanf("%d %d", &numbers[0], &numbers[1]);
        
        int client = socket(AF_UNIX, SOCK_STREAM, 0);
        connect(client, (struct sockaddr*)&addr, sizeof(addr));
        write(client, numbers, sizeof(numbers));
        close(client);
        printf("First child: Numbers sent through socket\n");
        exit(0);
    }
    else {
        wait(&status);
        
        pid2 = fork();
        
        if (pid2 == 0) {
            int numbers[2];
            int client = accept(sock, NULL, NULL);
            read(client, numbers, sizeof(numbers));
            close(client);
            
            printf("Second child: Calculating sum\n");
            printf("Sum: %d\n", numbers[0] + numbers[1]);
            exit(0);
        }
        else {
            wait(&status);
            close(sock);
            unlink(SOCKET_PATH);
            printf("Parent: All done\n");
        }
    }
    return 0;
}
