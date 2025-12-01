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

#include <signal.h>
#include <unistd.h>

int serverSockFd;
int shouldHandle = 1;
int maxClients;
int currentClients = 0;
pthread_mutex_t currentClientsMutex;
ClientList* clients;

void handleSigintServer() {
    shutdown(serverSockFd, SHUT_RDWR);
    close(serverSockFd);
    exit(0);
}
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
void initServer(int socket, int clientsAllowed) {
    signal(SIGINT, handleSigintServer);

    //initiaizes global variables and thread
    serverSockFd = socket;
    maxClients = clientsAllowed;
    clients = CreateClientList(maxClients);
    //init mutex for currentClients
    pthread_mutex_init(&currentClientsMutex, NULL);

    //creates new thread to handle connection requests
    pthread_t tid;
    pthread_create(&tid, NULL, handleConnections, NULL);
    pthread_join(tid, NULL);
}
void* handleConnections(void* data) {
    while (1) {
        //creates socket for client
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        int clientSocket = accept(serverSockFd, (struct sockaddr*)&client_addr, &client_addr_size); // yeilds thread
        pthread_t tid;
        pthread_create(&tid, NULL, handleConnectionRequest, &clientSocket);
        pthread_detach(tid);
    }
    return NULL;
}
void* handleConnectionRequest(void* data) {
    int socket = *(int*)data;
    uint8_t buffer[1024];
    ssize_t dataSize = recv(socket, buffer, sizeof(buffer), 0);
    if (dataSize > 0 && currentClients < maxClients) {
        //processes clients join message
        Message connectionRequest = Deserialize(buffer, dataSize);
        Client* client;
        int accepted = ServerReceiveJoinRequest(socket, clients, &connectionRequest, &client);
        if (accepted == 1) {
            //lock and unlock mutex to prevent race condition in currentClients
            pthread_mutex_lock(&currentClientsMutex);
            currentClients += 1;
            pthread_mutex_unlock(&currentClientsMutex);
            //create  thread to handle client messaging
            pthread_t tid;
            pthread_create(&tid, NULL, handleClient, client);
            pthread_detach(tid);
        } else {
            //tell client fuck off
            shutdown(socket, SHUT_RDWR);
            close(socket);
        }
    } else {
        //tell client fuck off
        char reason[] = "SERVER IS FULL";
        ServerSendRejectMessage(socket, reason, sizeof(reason));
        shutdown(socket, SHUT_RDWR);
        close(socket);
    }
    //thread terminates after work is done, handleClient still runs
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
    ServerReceiveDisconnectRequest(client, clients);
    shutdown(client->clientFd, SHUT_RDWR);
    close(client->clientFd);
    removeClientFromList(clients, client);

    //lock and unlock mutex to prevent race condition in currentClients
    pthread_mutex_lock(&currentClientsMutex);
    currentClients -= 1;
    pthread_mutex_unlock(&currentClientsMutex);
    return NULL;
}
void ProcessRequest(Message* receivedMessage, Client* client) {
    //calls fucntions based on header
    if (strcmp((char*)receivedMessage->header, "SEND GLOBAL") == 0) ServerReceiveGlobalMessage(client, clients, receivedMessage);
    if (strcmp((char*)receivedMessage->header, "SEND PRIVATE") == 0) ServerReceivePrivateMessage(client, clients, receivedMessage);
}