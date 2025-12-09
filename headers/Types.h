//
// Created by kroff on 11/19/2025.
//

#ifndef CHAT_C_TYPES_H
#define CHAT_C_TYPES_H
#define MAX_LENGTH 64
#define MAX_STRINGS 64
#pragma once
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>

#endif //CHAT_C_TYPES_H

typedef struct {
    int clientFd;
    char name[24];
    int color;
    int isAllowed;
} Client;

typedef struct {
    int size;
    int capacity;
    Client** clientBuffer;
    pthread_mutex_t mutexLock;
} ClientList;

typedef struct {
    time_t timeStamp;
    int color;

    char senderName[24];
    char recipientName[24];
    char header[24];
    char body[256];
} Message;

typedef struct {
    int maxClients;
    int serverFd;
    int doConnections;

    char serverPass[24];
    char serverBlacklist[MAX_STRINGS][MAX_LENGTH];

    FILE* censorFile;
    ClientList* clientList;
    pthread_mutex_t serverDataMutex;
} ServerData;

typedef struct {
    int clientFd;
    char name[24];
} ClientData;