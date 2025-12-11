//
// Created by kroff on 11/19/2025.
//

#include "../headers/Clients.h"
#include <stdlib.h>
#include <string.h>

ClientList* CreateClientList(int capacity) {
    //creates client list object to hold client objects
    ClientList* clientList = (ClientList*)malloc(sizeof(ClientList));
    clientList->clientBuffer = calloc(sizeof(Client*), capacity);
    clientList->capacity = capacity;
    clientList->size = 0;
    pthread_mutex_init(&clientList->mutexLock, NULL);
    return clientList;
}
Client* CreateClient(int clientFd, char clientName[], int clientColor, struct sockaddr_in* ipAddress, int muteState) {
    //creates client object, then return
    Client* client = (Client*)malloc(sizeof(Client));
    client->clientFd = clientFd;
    client->color = clientColor;
    client->isMuted = muteState;
    strcpy(client->name, clientName);
    client->clientAddr = *ipAddress;
    return client;
}
void addClientToList(ClientList* clientList, Client* client) {
    //make sure list isnt full
    if (clientList->size >= clientList->capacity) return;

    pthread_mutex_lock(&clientList->mutexLock);
    clientList->clientBuffer[clientList->size] = client;
    clientList->size++;
    pthread_mutex_unlock(&clientList->mutexLock);
}

void removeClientFromList(ClientList* client_list, Client* client) {
    pthread_mutex_lock(&client_list->mutexLock);

    //find index of desired client to remove
    int targetIndex = -1;
    for (int i = 0; i < client_list->size; i++) {
        if (client_list->clientBuffer[i] == client) {
            targetIndex = i;
        }
    }
    //make sure client is in list
    if (targetIndex == -1) return;
    //free client memory and shift list
    //then we decrement length
    free(client_list->clientBuffer[targetIndex]);
    for (int i = targetIndex; i < client_list->size; i++) {
        client_list->clientBuffer[i] = client_list->clientBuffer[i + 1];
    }
    client_list->size -= 1;
    pthread_mutex_unlock(&client_list->mutexLock);
}
