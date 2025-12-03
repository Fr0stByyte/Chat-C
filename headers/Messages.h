//
// Created by kroff on 11/19/2025.
//

#ifndef CHAT_C_MESSAGES_H
#define CHAT_C_MESSAGES_H

#endif //CHAT_C_MESSAGES_H
#include "Types.h"
#define BLACK   "\x1b[30m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define WHITE   "\x1b[37m"

#define BRIGHT_BLACK   "\x1b[90m"
#define BRIGHT_RED     "\x1b[91m"
#define BRIGHT_GREEN   "\x1b[92m"
#define BRIGHT_YELLOW  "\x1b[93m"
#define BRIGHT_BLUE    "\x1b[94m"
#define BRIGHT_MAGENTA "\x1b[95m"
#define BRIGHT_CYAN    "\x1b[96m"
#define BRIGHT_WHITE   "\x1b[97m"

#define RESET   "\x1b[0m"

static const char* colorArray[] = {
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,

    BRIGHT_BLACK,
    BRIGHT_RED,
    BRIGHT_GREEN,
    BRIGHT_YELLOW,
    BRIGHT_BLUE,
    BRIGHT_MAGENTA,
    BRIGHT_CYAN,
    BRIGHT_WHITE
};

/**
 * serializes a message object into an array, must be used for transmission over sockets
 * @param[in] message pointer to message that will be serialized
 * @param[out] data pointer to array that will hold the serialized data
 */
void Serialize(Message* message, uint8_t data[1024]);

/**
 * deserializes data from an array back into a message structure. used after data is recieved from a socket
 * @param[in] data pointer to array where serialized data is stored
 * @param[in] dataSize size of data that should be deserialized
 * @param[out] signal pointer for variable that will store errors
 * @return message struct with deserialized values
 */
Message Deserialize(uint8_t data[1024], ssize_t dataSize);

/**
 * creates a message struct storing the correctly formated parameters. allocated on the stack.
 * @param[in] timeStamp time stamp that tells when the message was sent
 * @param senderLength length of name of sender
 * @param recipientLength length of name of recipient
 * @param[in] headerLength size of the header array (be sure to use sizeof(), not strlen()
 * @param[in] bodyLength size of body array (be sure to use sizeof(), not strlen()
 * @param[in] header array that stores the header, should be null terminated
 * @param[in] body array that stores the body, should be null terminated
 * @return message struct contain properly formatted data from parameters
 */
Message createMessage(uint32_t timeStamp, uint32_t senderLength, uint32_t recipientLength, uint32_t headerLength, uint32_t bodyLength, uint32_t color, uint8_t sender[], uint8_t recipient[],  uint8_t header[], uint8_t body[]);
/**
 * clears a buffer, probs won't use tho
 * @param[in] buffer array storing data to be cleared
 * @param[in] size size of buffer
 */
void ClearBuffer(uint8_t buffer[1024], size_t size);