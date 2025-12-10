//
// Created by kroff on 12/2/2025.
//
#include <stdio.h>
#include <string.h>

#include "../headers/Client.h"
#include "../headers/Messages.h"
void processMessage(Message* message) {
    if (strcmp(message->header, "RECEIVE GLOBAL") == 0) {
        const char* color = colorArray[message->color];
        printf("%s" "[%s]: %s" RESET "\n", color, message->senderName, message->body);
    }
    if (strcmp(message->header, "REJECT ACTION") == 0) {
        printf(RED "SERVER REJECTED ACTION: %s" RESET "\n", message->body);
    }
    if (strcmp(message->header, "RECEIVE PRIVATE") == 0) {
        const char* color = colorArray[message->color];
        printf("%s" "[PRIVATE][%s]: %s" RESET "\n", color, message->senderName, message->body);
    }
}