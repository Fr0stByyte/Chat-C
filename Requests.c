//
// Created by kroff on 10/30/2025.
//


#include <pthread.h>

#include "Chat_C.h"
#include <string.h>
#include <time.h>


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

    //sereilize into buffer
    uint8_t buffer[1024];
    Serialize(&messageToSend, buffer);

    // send to all clients
    for (int i = 0; i < client_list->size; i++) {
        send(client_list->clientBuffer[i]->clientFd, buffer, sizeof(buffer), 0);
    }
}
void ClientReceiveGlobalMessage(Message* message) {
    //TODO this requires us to figure out how to display messages. Probably will make a client module or smthn.
}
void ServerReceiveJoinRequest(Client* client, ClientList* client_list, Message* message) {


    // gets client name and color from the connecting client
    // TODO: add checks for taken names
    strcpy(client->name, (char*)message->senderName);
    client->color = (int)message->color;
    client->isAllowed = 1;

    //adds client to global list of clients
    addClientToList(client_list, client);

    //create message to confirm client joining the room
    uint8_t header[] = "SUCCESS JOIN";
    uint8_t body[] = "joined the room!";
    uint8_t sender[] = "SERVER";
    Message messageToClient = createMessage(
        time(NULL),
        sizeof(sender),
        sizeof(message->recipientName),
        sizeof(header),
        sizeof(body),
        0,
        sender,
        message->recipientName,
        header,
        body
        );

    //serialize and send message
    uint8_t buffer2[1024];
    Serialize(&messageToClient, buffer2);
    send(client->clientFd, buffer2, sizeof(buffer2), 0);
}

void ServerReceiveDisconnectRequest(Client* client, ClientList* client_list) {
    //rempve client from list and end thread
    removeClientFromList(client_list, client);
    pthread_exit(NULL);
}

void ServerReceivePrivateMessage(Client* client, ClientList* client_list, Message* message) {

    //create message to send, essentially the message the client sent with a diffrent header
    uint8_t header[] = "RECEIVE PRIVATE";
    Message messageToSend = createMessage(
        time(NULL),
        sizeof(message->senderLength), //sender should be player, not server
        sizeof(message->recipientLength),
        sizeof(header),
        sizeof(message->body),
        message->color,
        message->senderName,
        message->recipientName,
        message->header,
        message->body
        );
    uint8_t buffer[1024];
    Serialize(&messageToSend, buffer);

    send(client->clientFd, buffer, sizeof(buffer), 0); //TODO: Fix so it gets send to the client it should be sent too
}