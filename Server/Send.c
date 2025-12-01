//
// Created by kroff on 11/30/2025.
//

#include "../headers/Server.h"

#include <stdio.h>
#include "../headers/Messages.h"

void ServerSendDirectMessage(Client* client, char* header, size_t headerSize, char* message, size_t messageSize) {
    uint8_t sender[] = "SERVER";
    Message messageToSend = createMessage(
        time(NULL),
        sizeof(sender),
        sizeof(client->name),
        headerSize,
        messageSize,
        0,
        sender,
        (uint8_t*)client->name,
        (uint8_t*)header,
        (uint8_t*)message
    );
    uint8_t buffer[1024];
    Serialize(&messageToSend, buffer);
    send(client->clientFd, buffer, sizeof(buffer), 0);
}

void ServerSendRejectMessage(int socket, char* reason, size_t reasonSize) {
    uint8_t sender[] = "SERVER";
    uint8_t header[] = "REJECT ACTION";
    Message messageToSend = createMessage(
        time(NULL),
        sizeof(sender),
        0,
        sizeof(header),
        reasonSize,
        0,
        sender,
        NULL,
        header,
        (uint8_t*)reason
    );
    uint8_t buffer[1024];
    Serialize(&messageToSend, buffer);
    send(socket, buffer, sizeof(buffer), 0);
}

void ServerSendGlobalMessage(ClientList* client_list, char* header, size_t headerSize, char* message, size_t message_size) {
    uint8_t sender[] = "SERVER";
    uint8_t recipient[] = "ALL";
    Message messageToClient = createMessage(
        time(NULL),
        sizeof(sender),
        sizeof(recipient),
        headerSize,
        message_size,
        0,
        sender,
        recipient,
        (uint8_t*)header,
        (uint8_t*)message
        );

    //serialize and send message
    uint8_t buffer[1024];
    Serialize(&messageToClient, buffer);

    for (int i = 0; i < client_list->size; i++) {
        send(client_list->clientBuffer[i]->clientFd, buffer, sizeof(buffer), 0);
    }
}
