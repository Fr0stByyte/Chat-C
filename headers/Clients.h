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
 * Allocates a new client object
 * @param clientFd clients socket
 * @param clientName name of client
 * @param clientColor color id of client
 * @param ipAddress client descriptor storing ip address
 * @param muteState tells server if client is muted or not
 * @return Client object
 */
Client* CreateClient(int clientFd, char* clientName, int clientColor, struct sockaddr_in* ipAddress, int muteState);
/**
 * creates a client list object on the heap
 * @param[in] capacity max amount of clients the list can store
 * @return pointer to newly created client list
 */
ClientList* CreateClientList(int capacity);

/**
 * removes a client from a clientlist
 * @param client_list client list to remove client on
 * @param client client that should be removed
 */
void removeClientFromList(ClientList* client_list, Client* client);

