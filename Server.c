//
// Created by kroff on 10/20/2025.
//


#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include "Chat_C.h"

int serverSockFd;
int doListen = 1;
ClientList* clients;

int createServerSocket(int port) {
    //creates sockaddr struct
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    //create file descriptor for use
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //do some checks
    if (sockfd == -1) return -1;
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) return -1;
    //listen on the socket for connections, non-blocking thankfully
    listen(sockfd, SOMAXCONN);
    return sockfd;
}
void initServer(int socket, int maxClients) {

    //initiaizes global variables and thread
    serverSockFd = socket;
    clients = CreateClientList(maxClients);

    pthread_t tid;
    pthread_create(&tid, NULL, handleConnections, NULL);
    pthread_join(tid, NULL);
}
void* handleConnections(void* data) {
    //TODO: add break condition
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);

        printf("listening on port:\n");
        int clientSocket = accept(serverSockFd, (struct sockaddr*)&client_addr, &client_addr_size); // yeilds thread
        printf("accepted client connection");
        Client* newClient = CreateClient(clientSocket, client_addr); // create user client
        pthread_t tid;
        pthread_create(&tid, NULL, handleClient, newClient); // spawn new thread to handle communication with client
        pthread_detach(tid);
    }
    return NULL;
}
void* handleClient(void* data) {
    Client* client = (Client*)data;
    uint8_t buffer[1024];
    ssize_t dataSize = recv(client->clientFd, buffer, sizeof(buffer), 0);
    Message clientMessage = Deserialize(buffer, dataSize);
    ProcessRequest(&clientMessage, client);
    return NULL;
}
void ProcessRequest(Message* receivedMessage, Client* client) {
    if (strcmp((char*)receivedMessage->header, "SEND GLOBAL") == 0) ServerReceiveGlobalMessage(client, clients, receivedMessage);
    if (strcmp((char*)receivedMessage->header, "REQUEST CONNECT") == 0) ServerReceiveJoinRequest(client, clients, receivedMessage);
    if (strcmp((char*)receivedMessage->header, "RECEIVE DISCONNECT") == 0) ServerReceiveDisconnectRequest(client, clients);
    if (strcmp((char*)receivedMessage->header, "SEND PRIVATE") == 0) ServerReceivePrivateMessage(client, clients, receivedMessage);
}
ClientList* CreateClientList(int capacity) {
    ClientList* clientList = (ClientList*)malloc(sizeof(ClientList));
    clientList->clientBuffer = calloc(sizeof(Client*), capacity);
    clientList->capacity = capacity;
    clientList->size = 0;
    pthread_mutex_init(&clientList->mutexLock, NULL);
    return clientList;
}
Client* CreateClient(int clientFd, struct sockaddr_in clientAddress) {
    Client* client = (Client*)malloc(sizeof(Client));
    client->clientFd = clientFd;
    client->clientAddr = clientAddress;
    client->isAllowed = 0;
    return client;
}
void addClientToList(ClientList* clientList, Client* client) {
    if (clientList->size >= clientList->capacity) return;

    pthread_mutex_lock(&clientList->mutexLock);
    clientList->clientBuffer[clientList->size] = client;
    clientList->size++;
    pthread_mutex_unlock(&clientList->mutexLock);
}

void removeClientFromList(ClientList* client_list, Client* client) {
    pthread_mutex_lock(&client_list->mutexLock);

    int targetIndex = -1;
    for (int i = 0; i < client_list->size; i++) {
        if (client_list->clientBuffer[i] == client) {
            targetIndex = i;
        }
    }
    if (targetIndex == -1) return;
    free(client_list->clientBuffer[targetIndex]);
    for (int i = targetIndex; i < client_list->size; i++) {
        client_list->clientBuffer[i] = client_list->clientBuffer[i + 1];
    }
    client_list->size -= 1;
    pthread_mutex_unlock(&client_list->mutexLock);
}