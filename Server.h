//
// Created by kroff on 10/20/2025.
//


#ifndef CHAT_C_UTIL_H
#define CHAT_C_UTIL_H
#include <netinet/in.h>

#endif //CHAT_C_UTIL_H

typedef struct {
    int doListen;
    int sockFd;
} ThreadData;

typedef struct {
    int clientFd;
    struct sockaddr_in clientAddr;
    char name[64];
    int color;
    ThreadData* threadData;

} ClientData;

int createServerSocket(int port);
void* handleClient(void* data);
void* handleConnections(void* data);