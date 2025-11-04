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
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);


    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) return -1;
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) return -1;
    listen(sockfd, SOMAXCONN);
    return sockfd;
}
void initServer(int socket, int maxClients) {
    serverSockFd = socket;
    clients = CreateClientList(maxClients);

    pthread_t tid;
    pthread_create(&tid, NULL, handleConnections, NULL);
    pthread_join(tid, NULL);
}
void* handleConnections(void* data) {
    while (doListen ==1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);

        printf("listening on port:\n");
        int clientSocket = accept(serverSockFd, (struct sockaddr*)&client_addr, &client_addr_size);
        printf("accepted client connection");
        Client* newClient = CreateClient(clientSocket, client_addr);
        pthread_t tid;
        pthread_create(&tid, NULL, handleClient, newClient);
        pthread_detach(tid);
    }
    return NULL;
}
void* handleClient(void* data) {
    Client* client = (Client*)data;
    uint8_t buffer[1024];
    ssize_t dataSize = recv(client->clientFd, buffer, sizeof(buffer), 0);
    int signal;
    Message clientMessage = Deserialize(buffer, dataSize, &signal);
    if (signal == 0) ProcessRequest(&clientMessage, client);
    return NULL;
}
void ProcessRequest(Message* receivedMessage, Client* client) {
    if (strcmp((char*)receivedMessage->header, "SEND GLOBAL") == 0) {
        uint8_t header[] = "RECEIVE GLOBAL";
        Message messageToSend = createMessage(time(NULL), sizeof(header), receivedMessage->bodyLength, header, receivedMessage->body);

        uint8_t buffer[1024];
        Serialize(&messageToSend, buffer);

        for (int i = 0; i < clients->size; i++) {
            send(clients->clientBuffer[i]->clientFd, buffer, sizeof(buffer), 0);
        }
    }
    if (strcmp((char*)receivedMessage->header, "REQUEST CONNECT") == 0) ReceiveJoinRequest(client, receivedMessage);
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