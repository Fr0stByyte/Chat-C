#include "../headers/Server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../headers/Clients.h"


void shutDownServer() {
    ServerData* serverData = getServerData();

    pthread_mutex_lock(&serverData->serverDataMutex);
    serverData->doConnections = 0;
    for (int i = 0; i < serverData->clientList->size; i++) {
        char body[] = "Server is shutdown!";
        ServerSendDirectMessage(serverData->clientList->clientBuffer[i], body);
        shutdown(serverData->clientList->clientBuffer[i]->clientFd, SHUT_RDWR);
        close(serverData->clientList->clientBuffer[i]->clientFd);
    }
    shutdown(serverData->serverFd, SHUT_RDWR);
    close(serverData->serverFd);
    free(serverData->clientList);
    pthread_mutex_unlock(&serverData->serverDataMutex);
    exit(0);
}

void kickPlr(Client* client, char* message) {
    ServerData* serverData = getServerData();

    pthread_mutex_lock(&serverData->serverDataMutex);

    char newMessage[256];
    strcpy(newMessage, "You have been kicked! Reason: ");
    strcat(newMessage, message);
    ServerSendDirectMessage(client, newMessage);
    shutdown(client->clientFd, SHUT_RDWR);
    close(client->clientFd);
    pthread_mutex_unlock(&serverData->serverDataMutex);
}

void muteIP(Client* client, char* message) {
    ServerData* serverData = getServerData();

    char newMessage[256];
    strcpy(newMessage, "You have been muted! Reason: ");
    strcat(newMessage, message);
    ServerSendDirectMessage(client, newMessage);
    appendIPList(serverData->muteList, &client->clientAddr.sin_addr);
    client->isMuted = 1;
}
void unMuteIP(Client* client, char* message) {
    ServerData* serverData = getServerData();
    char newMessage[256];
    strcpy(newMessage, "You have been unmuted! Reason: ");
    strcat(newMessage, message);
    ServerSendDirectMessage(client, newMessage);
    removefromIPList(serverData->muteList, &client->clientAddr.sin_addr);
    client->isMuted = 0;
}

void banIP(Client* client, char* message) {
    ServerData* serverData = getServerData();
    appendIPList(serverData->banList, &client->clientAddr.sin_addr);
    kickPlr(client, message);
}
void unBanIP(struct in_addr* address) {
    ServerData* serverData = getServerData();
    removefromIPList(serverData->banList, address);
}

void veiwLists() {
    ServerData* serverData = getServerData();
    printf("connected clients:\n");
    for (int i = 0; i < serverData->clientList->size; i++) {
        printf("[%d]: %s\n", i, serverData->clientList->clientBuffer[i]->name);
    }
    printf("muted IP's:\n");
    for (int i = 0; i < serverData->muteList->length; i++) {
        printf("[%d]: %s\n", i, serverData->muteList->buffer[i]);
    }
    printf("banned IP's:\n");
    for (int i = 0; i < serverData->banList->length; i++) {
        printf("[%d]: %s\n", i, serverData->banList->buffer[i]);
    }
}
int ProcessCommand(char* command, int argc, char* argv[]) {
    ServerData* serverData = getServerData();

    if (strcmp(command, "#shutdown") == 0) shutDownServer();
    if (strcmp(command, "#kick") == 0) {
        if (argc < 1) return 0;
        char message[24];
        printf("enter kick message: ");
        fgets(message, sizeof(message), stdin);

        for (int i = 0; i < argc; i++) {
            char* plrName = argv[i];
            Client* targetUser = NULL;
            for (int i2 = 0; i2 < serverData->clientList->size; i2++) {
                if (strcmp(plrName, serverData->clientList->clientBuffer[i2]->name) == 0) targetUser = serverData->clientList->clientBuffer[i2];
            }
            if (targetUser == NULL) return 0;
            kickPlr(targetUser, message);
            printf("kicked player: %s", plrName);
        }
        return 1;
    }
    if (strcmp(command, "#help") == 0) {
        printf("[INFO] for help, refer to the repo: https://github.com/Fr0stByyte/Chat-C\n");
        return 1;
    }
    if (strcmp(command, "#mute-ip") == 0) {
        if (argc < 1) return 0;
        char message[24];
        printf("enter mute reason: ");
        fgets(message, sizeof(message), stdin);

        char* plrName = argv[0];
        Client* targetUser = NULL;
        for (int i2 = 0; i2 < serverData->clientList->size; i2++) {
            if (strcmp(plrName, serverData->clientList->clientBuffer[i2]->name) == 0) targetUser = serverData->clientList->clientBuffer[i2];
        }
        if (targetUser == NULL) return 0;
        muteIP(targetUser, message);
        printf("muted player: %s\n", plrName);
        return 1;
    }
    if (strcmp(command, "#unmute-ip") == 0) {
        if (argc < 1) return 0;
        char message[24];
        printf("enter mute reason: ");
        fgets(message, sizeof(message), stdin);

        char* plrName = argv[0];
        Client* targetUser = NULL;
        for (int i2 = 0; i2 < serverData->clientList->size; i2++) {
            if (strcmp(plrName, serverData->clientList->clientBuffer[i2]->name) == 0) targetUser = serverData->clientList->clientBuffer[i2];
        }
        if (targetUser == NULL) return 0; // no user found
        unMuteIP(targetUser, message);
        printf("unmuted ip: %s\n", plrName);
        return 1;
    }
    if (strcmp(command, "#ban-ip") == 0) {
        if (argc < 1) return 0;
        char message[24];
        printf("enter ban reason: ");
        fgets(message, sizeof(message), stdin);

        char* plrName = argv[0];
        Client* targetUser = NULL;
        for (int i2 = 0; i2 < serverData->clientList->size; i2++) {
            if (strcmp(plrName, serverData->clientList->clientBuffer[i2]->name) == 0) targetUser = serverData->clientList->clientBuffer[i2];
        }
        if (targetUser == NULL) return 0;
        banIP(targetUser, message);
        printf("banned player: %s\n", plrName);
        return 1;
    }
    if (strcmp(command, "#unban-ip") == 0) {
        if (argc < 1) return 0;

       struct in_addr targetAddr;
        inet_pton(AF_INET, argv[0], &targetAddr);
        unBanIP(&targetAddr);
        printf("unbanned ip: %s\n", argv[0]);
        return 1;
    }
    if (strcmp(command, "#list") == 0) {
        veiwLists();
        return 1;
    }
    if (strcmp(command, "#passwd") == 0) {
        if (argc < 1) {
            char pass[] = "";
            pthread_mutex_lock(&serverData->serverDataMutex);
            strcpy(serverData->serverPass, pass);
            pthread_mutex_unlock(&serverData->serverDataMutex);
            printf("server pass removed.\n");
            return 1;
        }
        if (strlen(argv[0]) + 1 > 24) return 0;
        pthread_mutex_lock(&serverData->serverDataMutex);
        strncpy(serverData->serverPass, argv[0], 24);
        pthread_mutex_unlock(&serverData->serverDataMutex);
        printf("server pass set to: %s\n", serverData->serverPass);
        return 1;
    }
    return 0;
}
void* receiveCommands(void* data) {
    ServerData* serverData = getServerData();
    while (1) {
        char input[256];
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';
        if (input[0] == '#') {
            char* strToken = strtok(input, " ");
            int argc = 0;
            char* argv[16];

            char* command = strToken;
            strToken = strtok(NULL, " ");

            while (strToken != NULL) {
                argv[argc] = strToken;
                argc += 1;
                strToken = strtok(NULL, " ");
            }
            int result  = ProcessCommand(command, argc,argv);
            if (result == 0) printf("Command not found / failed :(\n");
        } else {
            ServerSendGlobalMessage(serverData->clientList, input);
        }
    }
    return NULL;
}