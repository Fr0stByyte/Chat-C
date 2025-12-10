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
    char header[] = "RECEIVE GLOBAL";

    //create message
    Message messageToSend = createMessage(
        message->timeStamp,
        client->color,
        client->name,
        message->recipientName,
        header,
        message->body
        );
    printf("%s" "[%s]: %s" RESET "\n", colorArray[client->color], client->name, (char*)messageToSend.body);

    //sereilize into buffer
    uint8_t buffer[1024];
    Serialize(&messageToSend, buffer);

    // send to all clients
    for (int i = 0; i < client_list->size; i++) {
        send(client_list->clientBuffer[i]->clientFd, buffer, sizeof(buffer), 0);
    }
}
int ServerReceiveJoinRequest(int socket, ClientList* client_list, Message* joinRequest, Client** clientReturn, char* serverPass) {
    // gets client name and color from the connecting client
    int nameAllowed = 1;

    if (strcmp(serverPass, joinRequest->body) != 0 && strcmp(serverPass, "") != 0) {
        //password is incorrect but is required
        char reason[] = "INCORRECT PASSWORD";
        ServerSendRejectMessage(socket, reason);
        return 0;
    }

    //checks if name is already taken
    for (int i =0; i < client_list->size; i++) {
        if (strcmp(joinRequest->senderName, client_list->clientBuffer[i]->name) == 0) nameAllowed = 0;
    }
    //name cant be server or blank
    if (strcmp(joinRequest->senderName, "SERVER") == 0 && strcmp(joinRequest->senderName, "") == 0) nameAllowed = 0;

    //refuses if name is taken or not allowed
    if (nameAllowed == 0) {
        char reason[] = "NAME TAKEN / INVALID NAME";
        ServerSendRejectMessage(socket, reason);
        return 0;
    }
    // client is accepted, will create client
    *clientReturn = CreateClient(socket, joinRequest->senderName, joinRequest->color);
    //adds client to global list of clients
    addClientToList(client_list, *clientReturn);
    //create message to confirm client joining the room
    char header[] = "NEW JOIN";
    ServerSendGlobalMessage(client_list, header, (char*)joinRequest->senderName);
    printf(YELLOW "[SERVER]: %s has joined the chatroom!" RESET "\n", (char*)joinRequest->senderName);
    return 1;
}

void ServerReceiveDisconnectRequest(Client* client, ClientList* client_list) {
    //rempve client from list and end thread
    char header[] = "NEW LEAVE";
    ServerSendGlobalMessage(client_list, header, client->name);
    printf(YELLOW "[SERVER]: %s has left the chatroom!" RESET "\n", client->name);
    removeClientFromList(client_list, client);
}
void ServerReceivePrivateMessage(Client* client, ClientList* client_list, Message* message) {


    //create message to send, essentially the message the client sent with a diffrent header
    //find targeted user
    Client* targetUser = NULL;
    for (int i = 0; i < client_list->size; i++) {
        // printf("recipientName = '%s'\n", (char*)message->recipientName);
        // printf("client name   = '%s'\n", client_list->clientBuffer[i]->name);
        if (strcmp((char*)message->recipientName, (char*)client_list->clientBuffer[i]->name) == 0) {
            // printf("found!\n");
            targetUser = client_list->clientBuffer[i];
        }
    }
    if (targetUser == NULL) {
        char serverMsg[] = "User does not exist!";
        ServerSendDirectMessage(client, serverMsg);
        return;
    }
    char header[] = "RECEIVE PRIVATE";
    Message messageToSend = createMessage(
        time(NULL),
        client->color,
        client->name,
        targetUser->name,
        header,
        (char*)message->body
    );
    printf("%s" "[PRIVATE][%s]->[%s]: %s" RESET "\n", colorArray[client->color], client->name, messageToSend.recipientName, messageToSend.body);
    uint8_t buffer[1024];
    Serialize(&messageToSend, buffer);
    send(targetUser->clientFd, buffer, sizeof(buffer), 0);
}

void ServerReceivePlayersRequest(Client* client) {
    ServerData* serverData = getServerData();
    pthread_mutex_lock(&serverData->serverDataMutex);
    char players[1024];
    sprintf(players, "%d", serverData->clientList->size);
    strcat(players, " Active Users: ");

    for (int i = 0; i < serverData->clientList->size; i++) {
        strncat(players, serverData->clientList->clientBuffer[i]->name, 24);
        strncat(players, ", ", 1);
    }
    ServerSendDirectMessage(client, players);
    pthread_mutex_unlock(&serverData->serverDataMutex);
}

void ServerReceiveColorRequest(Client* client, int color) {

    if (color < 0 || color > 15) {
        ServerSendDirectMessage(client, "invalid color id");
        return;
    }
    client->color = color;
    ServerSendDirectMessage(client, "color changed!");
}