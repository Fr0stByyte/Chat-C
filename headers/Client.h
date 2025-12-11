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
 * @param[in] clientName name the client will give to the server
 * @param[in] clientColor color the client will give to the server, displayed to other users
 * @param[in] joinPass Password the client will send to the server
 */
void initClient(int socket, char* clientName, int clientColor, char* joinPass);

/**
 * closes the clients socket and frees memory
 */
void closeClientConnection();

/**
 * process a message to determine how it is handled by the client (how it should be displayed)
 * @param[in] message message object to be processed
 */
void processMessage(Message* message);

/**
 * Sends a message to the server with the global header, intended to be used to send a message to all users
 * @param message message body to be sent
 * @param clientSocket socket message is sent to
 */
void ClientSendGlobalMessage(char* message, int clientSocket);

/**
 * Sends a client message to the server with the private header, intended for private messages.
 * @param message message body to be sent
 * @param receiver name of client who should receive the message
 * @param clientSocket socket message is sent to
 */
void ClientSendPrivateMessage(char* message, char* receiver, int clientSocket);

/**
 * Sends a request to the server to get the amount of active players and their names
 * @param clientSocket socket that request is sent over
 */
void ClientGetPlayersRequest(int clientSocket);

/**
 * send request to the server asking for the color to be changed
 * @param clientSocket socket that request is sent over
 * @param color color id the client wants to change to
 */
void ClientColorRequest(int clientSocket, int color);