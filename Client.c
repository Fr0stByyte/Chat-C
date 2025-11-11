//
// Created by kroff on 10/22/2025.
//

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include "Chat_C.h"

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
   uint8_t recipient[] = "SERVER";;

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
   if (returnedMessageSize == -1) printf("uh oh bad return");
   Message receivedMessage = Deserialize(buffer, returnedMessageSize);
   printf("connection successful? %s\n", (char*)receivedMessage.header);
}