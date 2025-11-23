//
// Created by kroff on 10/30/2025.
//


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "../headers/Server.h"
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../headers/Messages.h"
#include "../headers/Clients.h"


void ServerReceiveGlobalMessage(Client* client, ClientList* client_list, Message* message) {
    uint8_t header[] = "RECEIVE GLOBAL";

    //create the damn message, long function but it needs to exist
    Message messageToSend = createMessage(
        message->timeStamp,
        message->senderLength,
        message->recipientLength,
        sizeof(header),
        message->bodyLength,
        message->color,
        message->senderName,
        message->recipientName,
        header,
        message->body
        );
    printf("[%s]: %s\n", (char*)messageToSend.senderName, (char*)messageToSend.body);

    //sereilize into buffer
    uint8_t buffer[1024];
    Serialize(&messageToSend, buffer);

    // send to all clients
    for (int i = 0; i < client_list->size; i++) {
        send(client_list->clientBuffer[i]->clientFd, buffer, sizeof(buffer), 0);
    }
}
int ServerReceiveJoinRequest(int socket, ClientList* client_list, Message* message, Client** clientReturn) {
    // gets client name and color from the connecting client
    int nameAllowed = 1;
    for (int i =0; i < client_list->size; i++) {
        if (strcmp((char*)message->senderName, (char*)client_list->clientBuffer[i]->name) == 0) nameAllowed = 0;
    }
    if (nameAllowed == 0) {
        uint8_t header[] = "FAIL JOIN";
        uint8_t sender[] = "SERVER";
        Message messageToClient = createMessage(
            time(NULL),
            sizeof(sender),
            message->recipientLength,
            sizeof(header),
            0,
            0,
            sender,
            message->recipientName,
            header,
            NULL
            );
        uint8_t buffer[1024];
        Serialize(&messageToClient, buffer);
        send(socket, buffer, sizeof(buffer), 0);
        return 0;
    }
    *clientReturn = CreateClient(socket, (char*)message->senderName, (int)message->color);
    //adds client to global list of clients
    addClientToList(client_list, *clientReturn);

    //create message to confirm client joining the room
    uint8_t header[] = "NEW JOIN";
    uint8_t sender[] = "SERVER";
    uint8_t recipient[] = "ALL";
    Message messageToClient = createMessage(
        time(NULL),
        sizeof(sender),
        sizeof(recipient),
        sizeof(header),
        message->senderLength,
        0,
        sender,
        recipient,
        header,
        message->senderName
        );
    printf("[%s]: %s has joined the chatroom!\n", (char*)messageToClient.senderName, (char*)messageToClient.body);
    //serialize and send message
    uint8_t buffer[1024];
    Serialize(&messageToClient, buffer);
    for (int i = 0; i < client_list->size; i++) {
        send(client_list->clientBuffer[i]->clientFd, buffer, sizeof(buffer), 0);
    }
    return 1;
}

void ServerReceiveDisconnectRequest(Client* client, ClientList* client_list) {
    //rempve client from list and end thread
    uint8_t header[] = "NEW LEAVE";
    uint8_t sender[] = "SERVER";
    uint8_t recipient[] = "ALL";
    Message messageToSend = createMessage(
        time(NULL),
        sizeof(sender), //sender should be player, not server
        sizeof(recipient),
        sizeof(header),
        sizeof(client->name),
        0,
        sender,
        recipient,
        header,
        (uint8_t*)client->name
    );
    printf("[%s]: %s has left the chatroom!\n", (char*)messageToSend.senderName, (char*)messageToSend.body);
    uint8_t buffer[1024];
    Serialize(&messageToSend, buffer);
    for (int i = 0; i < client_list->size; i++) {
        send(client_list->clientBuffer[i]->clientFd, buffer, sizeof(buffer), 0);
    }
}

void ServerReceivePrivateMessage(Client* client, ClientList* client_list, Message* message) {

    //create message to send, essentially the message the client sent with a diffrent header
    uint8_t header[] = "RECEIVE PRIVATE";
    Message messageToSend = createMessage(
        time(NULL),
        message->senderLength, //sender should be player, not server
        message->recipientLength,
        sizeof(header),
        message->bodyLength,
        message->color,
        message->senderName,
        message->recipientName,
        header,
        message->body
        );
    printf("[PRIVATE][%s]: %s\n", (char*)messageToSend.senderName, (char*)messageToSend.body);
    uint8_t buffer[1024];
    Serialize(&messageToSend, buffer);
    //find targeted user
    for (int i = 0; i < client_list->size; i++) {
        // send(client_list->clientBuffer[i]->clientFd, buffer, sizeof(buffer), 0);
        if (strcmp((char*)message->recipientName, client_list->clientBuffer[i]->name) == 0) {
            send(client_list->clientBuffer[i]->clientFd, buffer, sizeof(buffer), 0);
        }
    }
}