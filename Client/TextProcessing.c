//
// Created by kroff on 12/2/2025.
//
#include <stdio.h>
#include <string.h>

#include "../headers/Client.h"
#include "../headers/Messages.h"
void processMessage(Message* message) {
    if (strcmp((char*)message->header, "RECEIVE GLOBAL") == 0) {
        const char* color = colorArray[(int)message->color];
        printf("%s" "[%s]: %s" RESET "\n", color, (char*)message->senderName, (char*)message->body);
    }
    if (strcmp((char*)message->header, "NEW JOIN") == 0) {
        printf(YELLOW "[%s]: %s has joined the chatroom!" RESET "\n", (char*)message->senderName, (char*)message->body);
    }
    if (strcmp((char*)message->header, "REJECT ACTION") == 0) {
        printf(RED "SERVER REJECTED ACTION: %s" RESET "\n", (char*)message->body);
        closeClientConnection();
    }
    if (strcmp((char*)message->header, "NEW LEAVE") == 0) {
        printf(YELLOW "[%s]: %s has left the chatroom!" RESET "\n", (char*)message->senderName, (char*)message->body);
    }
    if (strcmp((char*)message->header, "RECEIVE PRIVATE") == 0) {
        const char* color = colorArray[(int)message->color];
        printf("%s" "[PRIVATE][%s]: %s" RESET "\n", color, (char*)message->senderName, (char*)message->body);
    }
}