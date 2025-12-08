//
// Created by kroff on 11/19/2025.
//

#ifndef CHAT_C_TYPES_H
#define CHAT_C_TYPES_H
#pragma once
#include <netinet/in.h>
#include <pthread.h>

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
