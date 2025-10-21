//
// Created by kroff on 10/20/2025.
//

#include "Server.h"

#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

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
void* handleClient(void* data) {
    ClientData* client_data = (ClientData*)malloc(sizeof(ClientData));
    memcpy(client_data, data, sizeof(ClientData));

    char buffer[1024];
    send(client_data->clientFd, buffer, sizeof(buffer), 0 );
    free(client_data);
    return NULL;
}

void* handleConnections(void* data) {
    ThreadData* thread_data = (ThreadData*)data;
    while (thread_data->doListen == 1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);

        printf("listening...\n");
        int clientSocket = accept(thread_data->sockFd, (struct sockaddr*)&client_addr, &client_addr_size);

        ClientData client_data = {};
        client_data.clientAddr = client_addr;
        client_data.clientFd = clientSocket;
        client_data.threadData = thread_data;
        pthread_t tid;
        pthread_create(&tid, NULL, handleClient, &client_data);
    }
    return NULL;
}

