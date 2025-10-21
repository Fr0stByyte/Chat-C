//
// Created by kroff on 10/20/2025.
//

#include "Messages.h"

#include <string.h>


void Serialize(Message* message, uint8_t data[1024]) {
    u_int32_t timeStamp = htonl(message->timeStamp);
    u_int32_t headerLength = htonl(message->headerLength);
    u_int32_t messageLength = htonl(message->messageLength);


    memcpy(data, &timeStamp, 4);
    memcpy(data + 4, &headerLength, 4);
    memcpy(data + 8, &messageLength, 4);
    memcpy(data + 12, message->header, message->headerLength);
    memcpy(data + 12 + message->headerLength, message->messageBody, message->messageLength);
}
Message Deserialize(uint8_t data[1024]) {
    u_int32_t timeStamp;
    u_int32_t headerLength;
    u_int32_t messageLength;

    memcpy(&timeStamp, data, 4);
    memcpy(&headerLength, data + 4, 4);
    memcpy(&messageLength, data + 8, 4);

    Message message = {};
    message.timeStamp = ntohl(timeStamp);
    message.headerLength = ntohl(headerLength);
    message.messageLength = ntohl(messageLength);

    memcpy(message.header, data + 12, message.headerLength);
    memcpy(message.messageBody, data + 12 + message.headerLength, message.messageLength);

    return message;
}