//
// Created by kroff on 10/22/2025.
//

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "Chat_C.h"

void sendChatMessages(int socket, uint8_t clientName[], uint32_t clientColor) {
    char messageText[256];
    uint8_t header[] = "SEND GLOBAL";
    uint8_t recipient[] = "SERVER";

    while (1) {
        fgets(messageText, sizeof(messageText), stdin);
        messageText[strcspn(messageText, "\n")] = '\0'; // remove newline

        if (strcmp(messageText, "exit") == 0) {
            printf("Disconnecting...\n");
            break;
        }

        Message msg = createMessage(
            time(NULL),
            strlen((char*)clientName),
            strlen((char*)recipient),
            strlen((char*)header),
            strlen(messageText),
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

void* receiveMessages(void* arg) {
    int socket = *(int*)arg;
    uint8_t buffer[1024];

    while (1) {
        ssize_t size = recv(socket, buffer, sizeof(buffer), 0);
        if (size <= 0) {
            printf("Disconnected from server.\n");
            break;
        }

        Message msg = Deserialize(buffer, size);
        if (strcmp((char*)msg.header, "RECEIVE GLOBAL") == 0) {
            printf("[%s]: %s\n", msg.senderName, msg.body);
        } else if (strcmp((char*)msg.header, "SUCCESS JOIN") == 0) {
            printf("%s\n", msg.body);
        }
    }
    
    return NULL;
}

int createClientSocket(char ip[16], int port) {
   struct sockaddr_in server;
   server.sin_family = AF_INET;
   if (inet_pton(AF_INET, ip, &server.sin_addr) <= 0) return -1;
   server.sin_port = htons(port);

   int clientFd = socket(AF_INET, SOCK_STREAM, 0);
   if (clientFd == -1) return -1;

   int isConnected = connect(clientFd, (struct sockaddr*)&server, sizeof(server));
   if (isConnected == -1) return -1;
   return clientFd;
}
void initClient(int socket, uint32_t nameLength, uint8_t clientName[], uint32_t clientColor) {

   uint8_t header[] = "REQUEST CONNECT";
   uint8_t recipient[] = "SERVER";

   Message message = createMessage(time(NULL),
      nameLength,
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
   ClearBuffer(buffer, 1024);

   ssize_t returnedMessageSize = recv(socket, buffer, sizeof(buffer), 0);
   if (returnedMessageSize == -1) 
   {
      printf("BAD RETURN\n");
      return;
   }

   Message receivedMessage = Deserialize(buffer, returnedMessageSize);
   printf("CONNECTION SUCCESSFUL: %s\n", (char*)receivedMessage.header);

   pthread_t recvThread;
   pthread_create(&recvThread, NULL, receiveMessages, &socket);
   pthread_detach(recvThread);

   sendChatMessages(socket, clientName, clientColor);
}