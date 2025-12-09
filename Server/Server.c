//
// Created by kroff on 10/20/2025.
//


#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#include "../headers/Messages.h"
#include "../headers/Clients.h"
#include "../headers/Server.h"

#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>

ServerData serverData = {};
int lineCount;

int getBlacklist(FILE *file) {
    int i = 0;
    while (fgets(serverData.serverBlacklist[i], MAX_LENGTH, file))
    {
        serverData.serverBlacklist[i][strcspn(serverData.serverBlacklist[i], "\r\n")] = '\0';
        i++;
    }
    rewind(file);
    fclose(file);
    return i;
}

void handleSigintServer() {
    shutdown(serverData.serverFd, SHUT_RDWR);
    close(serverData.serverFd);
    for (int i = 0; i < serverData.clientList->size; i++) {
        free(serverData.clientList->clientBuffer[i]);
    }
    free(serverData.clientList);
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
void initServer(int socket, int clientsAllowed, char* password) {
    signal(SIGINT, handleSigintServer);

    FILE *file = fopen("censor.txt", "r");
    if (file == NULL) {
        perror("Could not create server: censor file not found!\n");
        return;
    };
    if (strlen(password) >= 24) {
        perror("Password is too long!\n");
        return;
    }


    //initiaizes global variables and thread
    strncpy(serverData.serverPass, password, 24);
    serverData.serverFd = socket;
    serverData.maxClients = clientsAllowed;
    serverData.clientList = CreateClientList(serverData.maxClients);
    serverData.censorFile = file;

    lineCount = getBlacklist(file);

    char hostname[256];
    struct hostent *hostData;
    gethostname(hostname, 256);
    hostData = gethostbyname(hostname);

    printf("listening on IPS: \n");
    for (int i = 0; hostData->h_addr_list[i] != NULL; i++) {
        printf("%s\n", inet_ntoa(*(struct in_addr*)hostData->h_addr_list[i]));
    }
    //init mutex for currentClients
    pthread_mutex_init(&serverData.serverDataMutex, NULL);

    //creates new thread to handle connection requests
    pthread_t connectionThread;
    pthread_create(&connectionThread, NULL, handleConnections, NULL);
    pthread_detach(connectionThread);
    printf("%d max clients allowed\n", serverData.maxClients);

    if (strcmp(serverData.serverPass, "") == 0) printf("No password is set!\n");
    pthread_t commandThread;
    pthread_create(&commandThread, NULL, receiveCommands, NULL);
    pthread_join(commandThread, NULL);
}
void* handleConnections(void* data) {
    while (1) {
        //creates socket for client
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        int clientSocket = accept(serverData.serverFd, (struct sockaddr*)&client_addr, &client_addr_size); // yeilds thread
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
    if (dataSize > 0 && serverData.clientList->size < serverData.maxClients) {
        //processes clients join message
        Message connectionRequest = Deserialize(buffer, dataSize);
        Client* client;
        int accepted = ServerReceiveJoinRequest(socket, serverData.clientList, &connectionRequest, &client, serverData.serverPass);
        if (accepted == 1) {
            //create  thread to handle client messaging
            pthread_t tid;
            pthread_create(&tid, NULL, handleClient, client);
            pthread_detach(tid);
        } else {
            //close connection
            shutdown(socket, SHUT_RDWR);
            close(socket);
        }
    } else {
        //send reject message then close connection
        char reason[] = "SERVER IS FULL";
        ServerSendRejectMessage(socket, reason);
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
    ServerReceiveDisconnectRequest(client, serverData.clientList);
    shutdown(client->clientFd, SHUT_RDWR);
    close(client->clientFd);
    return NULL;
}
void ProcessRequest(Message* receivedMessage, Client* client) {
    //calls fucntions based on header
    for(int i = 0; i < lineCount; i++)
    {
        if (strcasestr((char*)receivedMessage->body, serverData.serverBlacklist[i]) != NULL) {
            char header[] = "RECEIVE PRIVATE";
            char serverMsg[] = "phrase is blacklisted!";
            ServerSendDirectMessage(client, header, serverMsg);
            return;
        }
    }

    if (receivedMessage->color < 0 || receivedMessage->color > 15) {
        char header[] = "RECEIVE PRIVATE";
        char serverMsg[] = "message is illegal: invalid color";
        ServerSendDirectMessage(client, header, serverMsg);
        return;
    }

    if (strcmp((char*)receivedMessage->header, "SEND GLOBAL") == 0) ServerReceiveGlobalMessage(client, serverData.clientList, receivedMessage);
    if (strcmp((char*)receivedMessage->header, "SEND PRIVATE") == 0) ServerReceivePrivateMessage(client, serverData.clientList, receivedMessage);
}

ServerData* getServerData() {
    return &serverData;
}