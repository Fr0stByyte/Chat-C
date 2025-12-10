#include "../headers/Server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
    ServerSendDirectMessage(client, message);
    shutdown(client->clientFd, SHUT_RDWR);
    close(client->clientFd);
    pthread_mutex_unlock(&serverData->serverDataMutex);
}
int ProcessCommand(char* command, int argc, char* argv[]) {
    ServerData* serverData = getServerData();

    if (strcmp(command, "#shutdown") == 0) shutDownServer();
    if (strcmp(command, "#kick") == 0) {

        if (argc < 1) return 0;
        char message[24];
        char* plrName = argv[0];

        Client* targetUser = NULL;
        for (int i = 0; i < serverData->clientList->size; i++) {
            if (strcmp(plrName, serverData->clientList->clientBuffer[i]->name) == 0) targetUser = serverData->clientList->clientBuffer[i];
        }
        if (targetUser == NULL) return 0;
        printf("enter kick message: ");
        fgets(message, sizeof(message), stdin);
        kickPlr(targetUser, message);
        return 1;
    }
    return 0;
}
void* receiveCommands(void* data) {
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
        }
    }
    return NULL;
}