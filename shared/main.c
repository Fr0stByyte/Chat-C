//
// Created by kroff on 10/17/2025.
#include <stdio.h>
#include <string.h>

#include "../headers/Client.h"
#include "../headers/Server.h"
#include "../headers/Messages.h"

void handleJoin() {
    char ip[16];
    char name[24];
    int color;

    printf("enter the ip of the server you wish to connect to: ");
    fgets(ip, 16, stdin);
    ip[strcspn(ip, "\n")] = '\0';
    printf("enter your username: ");
    fgets(name, 24, stdin);
    name[strcspn(name, "\n")] = '\0';
    printf("type an integer that cooresponds to your desired text color:\n");
    for (int i = 0; i < 16; i++) {
        printf("%d %s" "\n", i, colorArray[i]);
    }
    scanf("%d", &color);

    int socket = createClientSocket(ip);
    if (socket > 0) {
        initClient(socket, name, color - 1);
    } else {
        printf("socket creation failed!\n");
    }
}

void handleCreate() {
    int maxClients;
    printf("enter the max client number: ");
    scanf("%d", &maxClients);

    char fileName[24];
    printf("Enter the file name for word blacklist: ");
    scanf("%s", fileName);

    printf("listening for connections on port: 8080; %d max clients allowed...\n", maxClients);
    int sock = createServerSocket();
    initServer(sock, maxClients, fileName);
}

int main() {
    while (1) {
        char choice[24];
        printf("Welcome to Chat-C! type 'join' to join a room, or 'create' to create a room! Type 'exit' to exit the program! ");
        fgets(choice, 24, stdin);
        choice[strcspn(choice, "\n")] = '\0';
        if (strcmp(choice, "join") == 0) handleJoin();
        if (strcmp(choice, "create") == 0) handleCreate();
        if (strcmp(choice, "exit") == 0) break;
    }
    printf("Exiting!\n");
    return 0;
}
