//
// Created by kroff on 11/19/2025.
//

#ifndef CHAT_C_CLIENTS_H
#define CHAT_C_CLIENTS_H
#include "Types.h"

#endif //CHAT_C_CLIENTS_H

/**
 * adds a client to the clientList, probably thread safe...
 * @param[in] clientList pointer to list of clients (should be a global variable for multi-thread acess
 * @param[in] client pointer to the client object which will be added
 */
void addClientToList(ClientList* clientList, Client* client);
/**
 * Creates a client object on the heap
 * @param[in] clientFd clients socket
 * @param[in] clientAddress stores client data such as ip
 * @return pointer to newly created client object
 */
Client* CreateClient(int clientFd, char* clientName, int clientColor, struct sockaddr_in* ipAddress, int muteState);
/**
 * creates a client list object on the heap
 * @param[in] capacity max amount of clients the list can store
 * @return pointer to newly created client list
 */
ClientList* CreateClientList(int capacity);
void removeClientFromList(ClientList* client_list, Client* client);

