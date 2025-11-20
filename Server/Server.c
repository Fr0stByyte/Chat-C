//
// Created by kroff on 10/20/2025.
//


#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>

#include "../headers/Messages.h"
#include "../headers/Clients.h"
#include "../headers/Server.h"

int serverSockFd;
int shouldHandle = 1;
ClientList* clients;

int createServerSocket() {
    //creates sockaddr struct
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

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

    //creates new thread to handle connection requests
    pthread_t tid;
    pthread_create(&tid, NULL, handleConnections, NULL);
    pthread_join(tid, NULL);
}
void* handleConnections(void* data) {
    while (shouldHandle == 1) {
        //creates socket for client
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        int clientSocket = accept(serverSockFd, (struct sockaddr*)&client_addr, &client_addr_size); // yeilds thread
        //creates client object
        Client* newClient = CreateClient(clientSocket, client_addr); // create user client

        //creates new thread to handle communication with client
        pthread_t tid;
        pthread_create(&tid, NULL, handleClient, newClient);
        pthread_detach(tid);
    }
    return NULL;
}
void* handleClient(void* data) {
    Client* client = (Client*)data;
    uint8_t buffer[1024];
    ssize_t dataSize;
    while((dataSize = recv(client->clientFd, buffer, sizeof(buffer), 0)) > 0)
    {
        //converts message to readable object, then will handle request based on header
        Message clientMessage = Deserialize(buffer, dataSize);
        ProcessRequest(&clientMessage, client);
    }
    return NULL;
}
void ProcessRequest(Message* receivedMessage, Client* client) {
    //calls fucntions based on header
    if (strcmp((char*)receivedMessage->header, "SEND GLOBAL") == 0) ServerReceiveGlobalMessage(client, clients, receivedMessage);
    if (strcmp((char*)receivedMessage->header, "REQUEST CONNECT") == 0) ServerReceiveJoinRequest(client, clients, receivedMessage);
    if (strcmp((char*)receivedMessage->header, "SEND DISCONNECT") == 0) ServerReceiveDisconnectRequest(client, clients);
    if (strcmp((char*)receivedMessage->header, "SEND PRIVATE") == 0) ServerReceivePrivateMessage(client, clients, receivedMessage);
}