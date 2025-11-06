//
// Created by kroff on 10/30/2025.
//


#include <pthread.h>

#include "Chat_C.h"
#include <string.h>
#include <time.h>


void ServerReceiveGlobalMessage(Client* client, ClientList* client_list, Message* message) {
    uint8_t header[] = "RECEIVE GLOBAL";
    Message messageToSend = createMessage(time(NULL), sizeof(header), message->bodyLength, header, message->body);

    uint8_t buffer[1024];
    Serialize(&messageToSend, buffer);

    for (int i = 0; i < client_list->size; i++) {
        send(client_list->clientBuffer[i]->clientFd, buffer, sizeof(buffer), 0);
    }
}
void ClientReceiveGlobalMessage(Message* message) {

}
void ServerReceiveJoinRequest(Client* client, ClientList* client_list, Message* message) {
    uint8_t name[24];
    uint32_t nameLength;
    uint32_t color;
    memcpy(&color, message->body, 4);
    memcpy(&nameLength, message->body + 4, 4);
    memcpy(name, message->body + 8, ntohl(nameLength));


    strcpy(client->name, (char*)name);
    client->color = (int)ntohl(color);
    client->isAllowed = 1;

    addClientToList(client_list, client);

    uint8_t header[] = "SUCCESS JOIN";
    uint8_t body[] = "joined the room!";
    Message messageToClient = createMessage(time(NULL), sizeof(header), sizeof(body), header, body);

    uint8_t buffer2[1024];
    Serialize(&messageToClient, buffer2);
    send(client->clientFd, buffer2, sizeof(buffer2), 0);
}

void ServerReceiveDisconnectRequest(Client* client, ClientList* client_list) {
    removeClientFromList(client_list, client);
    pthread_exit(NULL);
}

void ServerReceivePrivateMessage(Client* client, ClientList* client_list, Message* message) {
    uint8_t header[] = "RECEIVE PRIVATE";
    Message messageToSend = createMessage(time(NULL), sizeof(header), message->bodyLength, header, message->body);
}