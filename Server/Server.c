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

#include <errno.h>
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

void handleSigINT() {
    pthread_mutex_lock(&serverData.serverDataMutex);
    serverData.doConnections = 0;
    for (int i = 0; i < serverData.clientList->size; i++) {
        shutdown(serverData.clientList->clientBuffer[i]->clientFd, SHUT_RDWR);
        close(serverData.clientList->clientBuffer[i]->clientFd);
        free(serverData.clientList->clientBuffer[i]);
    }
    free(serverData.clientList);
    shutdown(serverData.serverFd, SHUT_RDWR);
    close(serverData.serverFd);
    pthread_mutex_unlock(&serverData.serverDataMutex);
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
    if (sockfd < 0) {
        printf(RED "Failed to create socket. Error code: %d" RESET "\n", errno);
        return -1;
    }
    int opt = 1;
    //allows for quick rebinding, was a pain to figure out :(
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf(RED "Failed to bind socket. Error code: %d" RESET "\n", errno);
        if (errno == EADDRINUSE) printf("(Address is already in use, a server is running already on your machine.)\n");
        return -1;
    };
    //listen on the socket for connections, non-blocking thankfully
    if (listen(sockfd, SOMAXCONN) < 0) {
        printf("Failed to listen. Error code: %d\n", errno);
        return -1;
    };
    printf("created socket!\n");
    return sockfd;
}
void initServer(int socket, int clientsAllowed, char* password) {
    signal(SIGINT, handleSigINT);

    FILE *file = fopen("censor.txt", "r");
    if (file == NULL) {
        printf(RED "Could not create server: censor file not found!" RESET "\n");
        return;
    };
    if (strlen(password) >= 24) {
        printf(RED "Password is too long!" RESET "\n");
        return;
    }


    //initiaizes global variables and thread
    // no mutex locking here since this is intialization and this always gets called here first
    serverData.doConnections = 1;
    strncpy(serverData.serverPass, password, 24);
    serverData.serverFd = socket;
    serverData.maxClients = clientsAllowed;
    serverData.clientList = CreateClientList(serverData.maxClients);
    serverData.censorFile = file;
    //init mutex for currentClients
    pthread_mutex_init(&serverData.serverDataMutex, NULL);

    lineCount = getBlacklist(file);

    char hostname[256];
    gethostname(hostname, 256);
    struct hostent *hostData = gethostbyname(hostname);

    printf("listening on IPS: \n");
    for (int i = 0; hostData->h_addr_list[i] != NULL; i++) {
        printf("%s\n", inet_ntoa(*(struct in_addr*)hostData->h_addr_list[i]));
    }
    if (strcmp(serverData.serverPass, "") == 0) printf("No password is set!\n");
    //creates new thread to handle connection requests
    pthread_t connectionThread;
    pthread_create(&connectionThread, NULL, handleConnections, NULL);
    pthread_detach(connectionThread);
    printf("%d max clients allowed\n", serverData.maxClients);

    //creates new thread to handle commands
    pthread_t commandThread;
    pthread_create(&commandThread, NULL, receiveCommands, NULL);
    pthread_join(commandThread, NULL);
}
void* handleConnections(void* data) {
    while (serverData.doConnections == 1) {
        //creates socket for client
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        int clientSocket = accept(serverData.serverFd, (struct sockaddr*)&client_addr, &client_addr_size); // yeilds thread
        if (clientSocket < 0) {
            printf(RED "Could not accept connection: %d" RESET "\n", errno);
            if (errno == EINVAL) printf("(One of the arguments for accept() is invalid, ignore if this appears after shutdown)\n");
        }
        pthread_t tid;
        pthread_create(&tid, NULL, handleConnectionRequest, &clientSocket);
        pthread_detach(tid);
    }
    return NULL;
}
void* handleConnectionRequest(void* data) {
    int clientSocket = *(int*)data;
    uint8_t buffer[1024];
    ssize_t dataSize = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (dataSize > 0 && serverData.clientList->size < serverData.maxClients) {
        //processes clients join message
        Message connectionRequest = Deserialize(buffer, dataSize);
        Client* client;
        int accepted = ServerReceiveJoinRequest(clientSocket, serverData.clientList, &connectionRequest, &client, serverData.serverPass);
        if (accepted == 1) {
            //create  thread to handle client messaging
            pthread_t tid;
            pthread_create(&tid, NULL, handleClient, client);
            pthread_detach(tid);
        } else {
            //close connection
            shutdown(clientSocket, SHUT_RDWR);
            close(clientSocket);
        }
    } else {
        //send reject message then close connection
        char reason[] = "SERVER IS FULL";
        ServerSendRejectMessage(clientSocket, reason);
        shutdown(clientSocket, SHUT_RDWR);
        close(clientSocket);
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
            char serverMsg[] = "phrase is blacklisted!";
            ServerSendDirectMessage(client, serverMsg);
            return;
        }
    }

    if (strcmp(receivedMessage->header, "SEND GLOBAL") == 0) ServerReceiveGlobalMessage(client, serverData.clientList, receivedMessage);
    if (strcmp(receivedMessage->header, "SEND PRIVATE") == 0) ServerReceivePrivateMessage(client, serverData.clientList, receivedMessage);
    if (strcmp(receivedMessage->header, "REQUEST PLAYERS") == 0) ServerReceivePlayersRequest(client);
    if (strcmp(receivedMessage->header, "REQUEST COLOR") == 0) ServerReceiveColorRequest(client, receivedMessage->color);
}

ServerData* getServerData() {
    return &serverData;
}