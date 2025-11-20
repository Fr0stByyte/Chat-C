//
// Created by kroff on 10/20/2025.
//

#include "../headers/Messages.h"

#include <string.h>

void Serialize(Message* message, uint8_t data[1024]) {
    // format decriptor variables to network protocol
    uint32_t timeStamp = htonl(message->timeStamp);
    uint32_t headerLength = htonl(message->headerLength);
    uint32_t messageLength = htonl(message->bodyLength);
    uint32_t senderLength = htonl(message->senderLength);
    uint32_t recipientLength = htonl(message->recipientLength);
    uint32_t color = htonl(message->color);

    //copy descriptor variables into a blank data buffer
    memcpy(data, &timeStamp, 4);
    memcpy(data + 4, &senderLength, 4);
    memcpy(data + 8, &recipientLength, 4);
    memcpy(data + 12, &headerLength, 4);
    memcpy(data + 16, &messageLength, 4);
    memcpy(data + 20, &color, 4);

    // copy data into buffer
    memcpy(data + 24, message->senderName, message->senderLength);
    memcpy(data + 24 + message->senderLength, message->recipientName, message->recipientLength);
    memcpy(data + 24 + message->senderLength + message->recipientLength, message->header, message->headerLength);
    memcpy(data + 24 + message->senderLength + message->recipientLength + message->headerLength, message->body, message->bodyLength);
}
Message Deserialize(uint8_t data[1024], ssize_t dataSize) {
    //initialize descriptor variables
    uint32_t timeStamp;
    uint32_t senderLength;
    uint32_t recipientLength;
    uint32_t headerLength;
    uint32_t messageLength;
    uint32_t color;

    //copy first 16 bytes of data to decriptor variables
    memcpy(&timeStamp, data, 4);
    memcpy(&senderLength, data + 4, 4);
    memcpy(&recipientLength, data + 8, 4);
    memcpy(&headerLength, data + 12, 4);
    memcpy(&messageLength, data + 16, 4);
    memcpy(&color, data + 20, 4);

    // assign struct decriptor values to system formatted desscriptor variables
    Message message = {};
    message.timeStamp = ntohl(timeStamp);
    message.senderLength = ntohl(senderLength);
    message.recipientLength = ntohl(recipientLength);
    message.headerLength = ntohl(headerLength);
    message.bodyLength = ntohl(messageLength);
    message.color = ntohl(color);

    //copy message data to struct using sizes determined by the descriptor variables
    memcpy(message.senderName, data + 24, message.senderLength);
    memcpy(message.recipientName, data + 24 + message.senderLength, message.recipientLength);
    memcpy(message.header, data + 24 + message.senderLength + message.recipientLength, message.headerLength);
    memcpy(message.body, data + 24 + message.senderLength + message.recipientLength + message.headerLength, message.bodyLength);

    return message;
}
void ClearBuffer(uint8_t buffer[1024], size_t size) {
    memset(buffer, 0, size);
}

Message createMessage(uint32_t timeStamp, uint32_t senderLength, uint32_t recipientLength, uint32_t headerLength, uint32_t bodyLength, uint32_t color, uint8_t sender[], uint8_t recipient[], uint8_t header[], uint8_t body[]) {
    Message message = {};
    message.timeStamp = timeStamp;
    message.senderLength = senderLength;
    message.recipientLength = recipientLength;
    message.headerLength = headerLength;
    message.bodyLength = bodyLength;
    message.color = color;

    memcpy(message.senderName, sender, message.senderLength);
    memcpy(message.recipientName, recipient, message.recipientLength);
    memcpy(message.header, header, message.headerLength);
    memcpy(message.body, body, message.bodyLength);


    return message;
}