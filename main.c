//
// Created by kroff on 10/17/2025.
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include "Messages.h"
#include "Server.h"

int main() {
    // while (1) {
    //     char choice[24];
    //     printf("Welcome to Chat-C! type 'join' to join a room, or 'create' to create a room!\n");
    //     scanf("%s", choice);
    //     if (strcmp(choice, "join") == 0) {
    //         char ip[24];
    //         char name[24];
    //         int color;
    //
    //         printf("enter the ip of the server you wish to connect to: ");
    //         scanf("%s", ip);
    //         printf("enter your username: ");
    //         scanf("%s", name);
    //         printf("enter the your color: ");
    //         scanf("%d", &color);
    //
    //     } else if (strcmp(choice, "create") == 0) {
    //
    //     } else if (strcmp(choice, "exit") == 0) break;
    //     else printf("invalid option, please try again!\n");
    // }
    int sock = createServerSocket(8080);
    printf("Socket created: %d\n", sock);
    ThreadData thread_data = {
        .doListen = 1,
        .sockFd = sock
    };
    pthread_t tid;
    pthread_create(&tid, NULL, handleConnections, &thread_data);
    pthread_join(tid, NULL);
    return 0;
}
