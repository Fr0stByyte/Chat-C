//
// Created by kroff on 10/20/2025.
//

#ifndef CHAT_C_MESSAGES_H
#define CHAT_C_MESSAGES_H
#include <netinet/in.h>

#endif //CHAT_C_MESSAGES_H


typedef struct {
     u_int32_t timeStamp;
     u_int32_t headerLength;
    char header[24];
    u_int32_t messageLength;
    char messageBody[1024];
} Message;

void Serialize(Message* message, uint8_t data[]);
Message Deserialize(uint8_t data[]);