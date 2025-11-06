//
// Created by kroff on 10/20/2025.
//

#include "Chat_C.h"

#include <string.h>

void Serialize(Message* message, uint8_t data[1024]) {
    uint32_t timeStamp = htonl(message->timeStamp);
    uint32_t headerLength = htonl(message->headerLength);
    uint32_t messageLength = htonl(message->bodyLength);


    memcpy(data, &timeStamp, 4);
    memcpy(data + 4, &headerLength, 4);
    memcpy(data + 8, &messageLength, 4);

    memcpy(data + 12, message->header, message->headerLength);
    memcpy(data + 12 + message->headerLength, message->body, message->bodyLength);
}
Message Deserialize(uint8_t data[1024], ssize_t dataSize) {
    uint32_t timeStamp;
    uint32_t headerLength;
    uint32_t messageLength;

    memcpy(&timeStamp, data, 4);
    memcpy(&headerLength, data + 4, 4);
    memcpy(&messageLength, data + 8, 4);

    Message message = {};
    message.timeStamp = ntohl(timeStamp);
    message.headerLength = ntohl(headerLength);
    message.bodyLength = ntohl(messageLength);

    if (12 + message.headerLength + message.bodyLength > dataSize) {
        return message;
    }

    memcpy(message.header, data + 12, message.headerLength);
    memcpy(message.body, data + 12 + message.headerLength, message.bodyLength);
    return message;
}
void ClearBuffer(uint8_t buffer[1024], size_t size) {
    memset(buffer, 0, size);
}

Message createMessage(uint32_t timeStamp, uint32_t headerLength, uint32_t bodyLength, uint8_t header[], uint8_t body[]) {
    Message message = {};
    message.timeStamp = timeStamp;
    message.headerLength = headerLength;
    message.bodyLength = bodyLength;

    memcpy(message.header, header, headerLength);
    memcpy(message.body, body, bodyLength);

    return message;
}