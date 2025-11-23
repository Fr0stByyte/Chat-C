//
// Created by kroff on 10/17/2025.
#include <stdio.h>
#include <string.h>

#include "../headers/Client.h"
#include "../headers/Server.h"

void handleJoin() {
    char ip[16];
    uint8_t name[24];
    uint32_t color;

    printf("enter the ip of the server you wish to connect to: ");
    scanf("%s", ip);
    printf("enter your username: ");
    scanf("%s", name);
    printf("enter the your color: ");
    scanf("%d", &color);

    int socket = createClientSocket(ip);
    if (socket > 0) {
        initClient(socket, sizeof(name), name, color);
    } else {
        printf("socket creation failed!\n");
    }
}

void handleCreate() {
    int maxClients;
    printf("enter the max client number: ");
    scanf("%d", &maxClients);

    printf("listening for connections on port: 8080; %d max clients allowed...\n", maxClients);
    int sock = createServerSocket();
    initServer(sock, maxClients);
}

int main() {
    while (1) {
        char choice[24];
        printf("Welcome to Chat-C! type 'join' to join a room, or 'create' to create a room! Type 'exit' to exit the program! ");
        scanf("%s", choice);
        if (strcmp(choice, "join") == 0) handleJoin();
        if (strcmp(choice, "create") == 0) handleCreate();
        if (strcmp(choice, "exit") == 0) break;
    }
    printf("Exiting!\n");
    return 0;
}
