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

#include <signal.h>

#include "../headers/Messages.h"
char clientName[24];
int nameSize;
int clientColor;
int connected = 0;
int clientSocket;

void closeClientConnection() {
    shutdown(clientSocket, SHUT_RDWR);
    close(clientSocket);
    connected = 0;
}
void handleSigintClient() {
    closeClientConnection();
    exit(0);
}
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
            char privateHeader[] = "SEND PRIVATE";

            printf("enter recepient name: ");
            fgets(recipient, sizeof(recipient), stdin);
            recipient[strcspn(recipient, "\n")] = '\0';

            printf("enter message: ");
            fgets(msg, sizeof(msg), stdin);
            msg[strcspn(msg, "\n")] = '\0';

            Message privateMessage = createMessage(
                time(NULL),
                clientColor,
                clientName,
                recipient,
                privateHeader,
                msg
            );
            uint8_t buffer[1024];
            Serialize(&privateMessage, buffer);
            send(socket, buffer, sizeof(buffer), 0);
        } else if (strcmp(messageText, "") == 0) {
            printf("No data...\n");
        } else{
            char recipient[] = "ALL";
            char header[] = "SEND GLOBAL";
            Message msg = createMessage(
                time(NULL),
                clientColor,
                clientName,
                recipient,
                header,
                messageText
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

        Message message = Deserialize(buffer, size);
        processMessage(&message);
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
    void initClient(int socket, char* username, int color) {
        clientSocket = socket;
        signal(SIGINT, handleSigintClient);
        connected = 1;
        char header[] = "REQUEST CONNECT";
        char recipient[] = "SERVER";
        clientColor = color;
        strcpy(clientName, username);

        Message message = createMessage(
            time(NULL),
            clientColor,
           clientName,
            recipient,
            header,
            ""
            );

        uint8_t buffer[1024];
        Serialize(&message, buffer);
        send(socket, buffer, sizeof(buffer), 0);

        pthread_t recvThread;
        pthread_create(&recvThread, NULL, receiveMessages, &socket);
        pthread_detach(recvThread);

        sendChatMessages(socket);
}