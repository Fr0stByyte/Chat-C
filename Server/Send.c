//
// Created by kroff on 11/30/2025.
//

#include "../headers/Server.h"

#include <stdio.h>
#include <string.h>

#include "../headers/Messages.h"

void ServerSendDirectMessage(Client* client, char* header, char* message) {
    char sender[] = "SERVER";
    Message messageToSend = createMessage(
        time(NULL),
        4,
        sender,
        client->name,
        header,
        message
    );
    uint8_t buffer[1024];
    Serialize(&messageToSend, buffer);
    send(client->clientFd, buffer, sizeof(buffer), 0);
}

void ServerSendRejectMessage(int socket, char* reason) {
    char sender[] = "SERVER";
    char header[] = "REJECT ACTION";
    Message messageToSend = createMessage(
        time(NULL),
        2,
        sender,
        "",
        header,
        reason
    );
    uint8_t buffer[1024];
    Serialize(&messageToSend, buffer);
    send(socket, buffer, sizeof(buffer), 0);
}

void ServerSendGlobalMessage(ClientList* client_list, char* header, char* message) {
    char sender[] = "SERVER";
    char recipient[] = "ALL";
    Message messageToClient = createMessage(
        time(NULL),
        0,
        sender,
        recipient,
        header,
        message
        );
    //serialize and send message
    uint8_t buffer[1024];
    Serialize(&messageToClient, buffer);

    for (int i = 0; i < client_list->size; i++) {
        send(client_list->clientBuffer[i]->clientFd, buffer, sizeof(buffer), 0);
    }
}
