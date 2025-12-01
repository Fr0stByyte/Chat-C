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
    uint32_t timeStamp;
    uint32_t senderLength;
    uint32_t recipientLength;
    uint32_t headerLength;
    uint32_t bodyLength;
    uint32_t color;

    uint8_t senderName[24];
    uint8_t recipientName[24];
    uint8_t header[24];
    uint8_t body[256];
} Message;
