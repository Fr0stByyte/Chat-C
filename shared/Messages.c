//
// Created by kroff on 10/20/2025.
//

#include "../headers/Messages.h"

#include <string.h>

void Serialize(Message* message, uint8_t data[1024]) {
    size_t headerLen = strlen(message->header) + 1;
    size_t bodyLen = strlen(message->body) + 1;
    size_t senderLen = strlen(message->senderName) + 1;
    size_t recipientLen = strlen(message->recipientName) + 1;


    // format decriptor variables to network protocol
    uint32_t netTimeStamp = htonl((int)message->timeStamp);
    uint32_t netColor = htonl(message->color);
    uint32_t netHeader = htonl(headerLen);
    uint32_t netBody = htonl(bodyLen);
    uint32_t netSender = htonl(senderLen);
    uint32_t netRecipient = htonl(recipientLen);

    //copy descriptor variables into a blank data buffer
    memcpy(data, &netTimeStamp, 4);
    memcpy(data + 4, &netSender, 4);
    memcpy(data + 8, &netRecipient, 4);
    memcpy(data + 12, &netHeader, 4);
    memcpy(data + 16, &netBody, 4);
    memcpy(data + 20, &netColor, 4);

    // copy data into buffer
    memcpy(data + 24, (uint8_t*)message->senderName, senderLen);
    memcpy(data + 24 + senderLen, (uint8_t*)message->recipientName, recipientLen);
    memcpy(data + 24 + senderLen + recipientLen, (uint8_t*)message->header, headerLen);
    memcpy(data + 24 + senderLen + recipientLen + headerLen, (uint8_t*)message->body, bodyLen);
}
Message Deserialize(uint8_t data[1024], ssize_t dataSize) {
    //initialize descriptor variables
    uint32_t timeStamp;
    uint32_t senderLength;
    uint32_t recipientLength;
    uint32_t headerLength;
    uint32_t messageLength;
    uint32_t color;

    //copy first 24 bytes of data to decriptor variables
    memcpy(&timeStamp, data, 4);
    memcpy(&senderLength, data + 4, 4);
    memcpy(&recipientLength, data + 8, 4);
    memcpy(&headerLength, data + 12, 4);
    memcpy(&messageLength, data + 16, 4);
    memcpy(&color, data + 20, 4);

    headerLength = ntohl(headerLength);
    messageLength = ntohl(messageLength);
    senderLength = ntohl(senderLength);
    recipientLength = ntohl(recipientLength);

    // assign struct decriptor values to system formatted desscriptor variables
    Message message = {};
    message.timeStamp = (time_t)ntohl(timeStamp);
    message.color = (int)ntohl(color);

    //copy message data to struct using sizes determined by the descriptor variables
    memcpy(message.senderName, data + 24, senderLength);
    memcpy(message.recipientName, data + 24 + senderLength, recipientLength);
    memcpy(message.header, data + 24 + senderLength + recipientLength, headerLength);
    memcpy(message.body, data + 24 + senderLength + recipientLength + headerLength, messageLength);

    return message;
}
void ClearBuffer(uint8_t buffer[1024], size_t size) {
    memset(buffer, 0, size);
}

Message createMessage(time_t timeStamp, int color, char* sender, char* recipient, char* header, char* body) {
    Message message = {};
    message.timeStamp = timeStamp;
    message.color = color;

    strncpy(message.senderName, sender, 24);
    strncpy(message.recipientName, recipient, 24);
    strncpy(message.header, header, 24);
    strncpy(message.body, body, 256);
    return message;
}