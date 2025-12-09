//
// Created by kroff on 11/3/2025.
//

#ifndef CHAT_C_CHAT_C_H
#define CHAT_C_CHAT_C_H
#include "Types.h"
#endif //CHAT_C_CHAT_C_H
/**
 * creates a socket and connects it to a server, used to establish a connection
 * @param[in] ip ip address the client will connect too
 * @return returns a socket if a sucessful connection was made, else returns -1
 */
int createClientSocket(char ip[16]);

/**
 * initializes the client and sends data to the server requesting to join the chatroom
 * @param[in] socket socket given by createClientSocket
 * @param[in] nameLength length of the username
 * @param[in] clientName name the client will give to the server
 * @param[in] clientColor color the client will give to the server, displayed to other users
 */
void initClient(int socket, char* clientName, int clientColor, char* joinPass);
void closeClientConnection();
void processMessage(Message* message);

void ClientSendGlobalMessage(char* message, int color, int clientSocket);
void ClientSendPrivateMessage(char* message, char* receiver, int color, int clientSocket);
