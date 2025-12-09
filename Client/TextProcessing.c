//
// Created by kroff on 12/2/2025.
//
#include <stdio.h>
#include <string.h>

#include "../headers/Client.h"
#include "../headers/Messages.h"
void processMessage(Message* message) {
    if (strcmp(message->header, "RECEIVE GLOBAL") == 0) {
        const char* color = colorArray[(int)message->color];
        printf("%s" "[%s]: %s" RESET "\n", color, message->senderName, message->body);
    }
    if (strcmp(message->header, "NEW JOIN") == 0) {
        printf(YELLOW "[%s]: %s has joined the chatroom!" RESET "\n", message->senderName, message->body);
    }
    if (strcmp(message->header, "REJECT ACTION") == 0) {
        printf(RED "SERVER REJECTED ACTION: %s" RESET "\n", message->body);
        closeClientConnection();
    }
    if (strcmp(message->header, "NEW LEAVE") == 0) {
        printf(YELLOW "[%s]: %s has left the chatroom!" RESET "\n", message->senderName, message->body);
    }
    if (strcmp(message->header, "RECEIVE PRIVATE") == 0) {
        const char* color = colorArray[(int)message->color];
        printf("%s" "[PRIVATE][%s]: %s" RESET "\n", color, message->senderName, message->body);
    }
    if (strcmp(message->header, "RECEIVE DATA") == 0) {
        printf(YELLOW "[%s]: DATA: %s" RESET "\n", message->senderName, message->body);
    }
}