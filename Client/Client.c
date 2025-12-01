//
// Created by kroff on 10/22/2025.
//

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "../headers/Client.h"
#include "../headers/Messages.h"
uint8_t clientName[24];
uint32_t nameSize;
uint32_t clientColor;
int connected = 0;

void sendChatMessages(int socket) {
    char messageText[256];
    while (connected == 1) {
        fgets(messageText, sizeof(messageText), stdin);
        messageText[strcspn(messageText, "\n")] = '\0'; // remove newline

        if (strcmp(messageText, "#exit") == 0) {
            shutdown(socket, SHUT_RDWR);
            close(socket);
            printf("Disconnected!\n");
            connected = 0;
            return;
        }
        if(strcmp(messageText, "#pm") == 0) {
            char recipient[24];
            char msg[256];
            uint8_t privateHeader[] = "SEND PRIVATE";
            printf("enter recepient name: ");
            fgets(recipient, sizeof(recipient), stdin);
            printf("enter message: ");
            fgets(msg, sizeof(msg), stdin);

            Message privateMessage = createMessage(
                time(NULL),
                nameSize,
                sizeof(recipient),
                sizeof(privateHeader),
                sizeof(msg),
                clientColor,
                clientName,
                (uint8_t*)recipient,
                privateHeader,
                (uint8_t*)msg
            );
            uint8_t buffer[1024];
            Serialize(&privateMessage, buffer);
            send(socket, buffer, sizeof(buffer), 0);
        } else if (strcmp(messageText, "") == 0) {
            printf("No data...\n");
        } else{
            uint8_t recipient[] = "ALL";
            uint8_t header[] = "SEND GLOBAL";
            Message msg = createMessage(
                time(NULL),
                nameSize,
                sizeof(recipient),
                sizeof(header),
                sizeof(messageText),
                clientColor,
                clientName,
                recipient,
                header,
                (uint8_t*)messageText
            );
            uint8_t buffer[1024];
            Serialize(&msg, buffer);
            send(socket, buffer, sizeof(buffer), 0);
        }
    }
}

void* receiveMessages(void* arg) {
    int socket = *(int*)arg;
    uint8_t buffer[1024];

    while (1) {
        ssize_t size = recv(socket, buffer, sizeof(buffer), 0);
        if (size <= 0) {
            if (connected == 1) {
                printf("Disconnected from server.\n");
                shutdown(socket, SHUT_RDWR);
                close(socket);
                connected = 0;
            }
            return NULL;
        }

        Message msg = Deserialize(buffer, size);
        if (strcmp((char*)msg.header, "RECEIVE GLOBAL") == 0 && strcmp((char*)msg.senderName, (char*)clientName) != 0) {
            printf("[%s]: %s\n", (char*)msg.senderName, (char*)msg.body);
        }
        if (strcmp((char*)msg.header, "NEW JOIN") == 0 && strcmp((char*)msg.senderName, (char*)clientName) != 0) {
            printf("[%s]: %s has joined the chatroom!\n", (char*)msg.senderName, (char*)msg.body);
        }
        if (strcmp((char*)msg.header, "REJECT ACTION") == 0 && strcmp((char*)msg.senderName, (char*)clientName) != 0) {
            printf("SERVER REJECTED ACTION: %s\n", (char*)msg.body);
            shutdown(socket, SHUT_RDWR);
            close(socket);
            connected = 0;
            return NULL;
        }
        if (strcmp((char*)msg.header, "NEW LEAVE") == 0 && strcmp((char*)msg.senderName, (char*)clientName) != 0) {
            printf("[%s]: %s has left the chatroom!\n", (char*)msg.senderName, (char*)msg.body);
        }
        if (strcmp((char*)msg.header, "RECEIVE PRIVATE") == 0 && strcmp((char*)msg.senderName, (char*)clientName) != 0) {
            printf("[PRIVATE][%s]: %s\n", (char*)msg.senderName, (char*)msg.body);
        }
    }
}

int createClientSocket(char ip[16]) {
   struct sockaddr_in server;
   server.sin_family = AF_INET;
   if (inet_pton(AF_INET, ip, &server.sin_addr) <= 0) return -1;
   server.sin_port = htons(8080);

   int clientFd = socket(AF_INET, SOCK_STREAM, 0);
   if (clientFd == -1) return -1;

   int isConnected = connect(clientFd, (struct sockaddr*)&server, sizeof(server));
   if (isConnected == -1) return -1;
   return clientFd;
}
void initClient(int socket, uint32_t userLength, uint8_t username[], uint32_t color) {
    connected = 1;
   uint8_t header[] = "REQUEST CONNECT";
   uint8_t recipient[] = "SERVER";
    memcpy(clientName, username, userLength);
    nameSize = userLength;
    clientColor = color;

   Message message = createMessage(time(NULL),
      nameSize,
      sizeof(recipient),
      sizeof(header),
      0,
      clientColor,
      clientName,
      recipient,
      header,
      NULL);

   uint8_t buffer[1024];
   Serialize(&message, buffer);
   send(socket, buffer, sizeof(buffer), 0);

   pthread_t recvThread;
   pthread_create(&recvThread, NULL, receiveMessages, &socket);
   pthread_detach(recvThread);

   sendChatMessages(socket);
}