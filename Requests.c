//
// Created by kroff on 10/30/2025.
//


#include "Chat_C.h"
#include <string.h>
#include <time.h>


void ServerReceiveGlobalMessage(Client* client, Message* message) {

}
void ClientReceiveGlobalMessage(Client* client, Message* message) {

}
void ReceiveJoinRequest(Client* client, Message* message) {
    uint8_t name[24];
    uint32_t nameLength;
    uint32_t color;
    memcpy(&color, message->body, 4);
    memcpy(&nameLength, message->body + 4, 4);
    memcpy(name, message->body + 8, ntohl(nameLength));


    strcpy(client->name, (char*)name);
    client->color = (int)ntohl(color);
    client->isAllowed = 1;

    // addClientToList(clients, client);

    uint8_t header[] = "SUCCESS JOIN";
    uint8_t body[] = "joined the room!";
    Message messageToClient = createMessage(time(NULL), sizeof(header), sizeof(body), header, body);

    uint8_t buffer2[1024];
    Serialize(&messageToClient, buffer2);
    send(client->clientFd, buffer2, sizeof(buffer2), 0);
}