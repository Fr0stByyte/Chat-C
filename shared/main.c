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
    char pass[24];

    printf("enter the ip of the server you wish to connect to: ");
    fgets(ip, 16, stdin);
    ip[strcspn(ip, "\n")] = '\0';

    printf("enter the server password (leave blank if there is no password): ");
    fgets(pass, 24, stdin);
    pass[strcspn(pass, "\n")] = '\0';

    printf("enter your username: ");
    fgets(name, 24, stdin);
    name[strcspn(name, "\n")] = '\0';

    printf("type an integer that cooresponds to your desired text color:\n");
    for (int i = 0; i < 16; i++) {
        printf("%d %s" "\n", i, colorArray[i]);
    }
    scanf("%d", &color);

    int socket = createClientSocket(ip);
    if (socket < 0) return;
    initClient(socket, name, color, pass);
}

void handleCreate() {

    char pass[24];
    printf("Enter the password (leave blank for no password): ");
    fgets(pass, 24, stdin);
    pass[strcspn(pass, "\n")] = '\0';

    int maxClients;
    printf("enter the max client number: ");
    scanf("%d", &maxClients);

    int sock = createServerSocket();
    if (sock < 0) return; //I already printed the error so we are good
    initServer(sock, maxClients, pass);
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