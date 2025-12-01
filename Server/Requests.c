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
int ServerReceiveJoinRequest(int socket, ClientList* client_list, Message* joinRequest, Client** clientReturn) {
    // gets client name and color from the connecting client
    int nameAllowed = 1;
    for (int i =0; i < client_list->size; i++) {
        if (strcmp((char*)joinRequest->senderName, (char*)client_list->clientBuffer[i]->name) == 0) nameAllowed = 0;
    }
    if (nameAllowed == 0) {
        char reason[] = "NAME TAKEN";
        ServerSendRejectMessage(socket, reason, sizeof(reason));
        return 0;
    }
    *clientReturn = CreateClient(socket, (char*)joinRequest->senderName, (int)joinRequest->color);
    //adds client to global list of clients
    addClientToList(client_list, *clientReturn);
    //create message to confirm client joining the room
    char header[] = "NEW JOIN";
    ServerSendGlobalMessage(client_list, header, sizeof(header), (char*)joinRequest->senderName, sizeof(joinRequest->senderName));
    printf("[SERVER]: %s has joined the chatroom!\n", (char*)joinRequest->senderName);
    return 1;
}

void ServerReceiveDisconnectRequest(Client* client, ClientList* client_list) {
    //rempve client from list and end thread
    char header[] = "NEW LEAVE";
    ServerSendGlobalMessage(client_list, header, sizeof(header), client->name, sizeof(client->name));
    printf("[SERVER]: %s has left the chatroom!\n", client->name);
    //freeing the client is handled by Server.c
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