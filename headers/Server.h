//
// Created by kroff on 11/19/2025.
//

#ifndef CHAT_C_SERVER_H
#define CHAT_C_SERVER_H

#endif //CHAT_C_SERVER_H
#include "Types.h"

/**
 * called when server receives a message to the client with the purpose of it being broadcast globally
 * @param client client object that message was received from
 * @param clientlist
 * @param message message struct conating message data client sent
 */
void ServerReceiveGlobalMessage(Client* client, ClientList* client_list, Message* message);
/**
 * handles request sent to client to connect to a room
 * @param client client that wishes to connect
 * @param clients
 * @param message connection data (text color, name)
 */
int ServerReceiveJoinRequest(int socket, ClientList* client_list, Message* message, Client** clientReturn, char* serverPass);
void ServerReceiveDisconnectRequest(Client* client, ClientList* client_list);
void ServerReceivePrivateMessage(Client* client, ClientList* client_list, Message* message);
/**
 * creates a complete socket from the given port, oskcet is bound and listening
 * @returns server's socket
 */
int createServerSocket();
/**
 * handles data coming in from a client, should be on its own thread.
 * @param[in] data pointer to a client object (casted to a void pointer)
 * @return
 */
void* handleClient(void* data);
/**
 * accepts incoming connections, then creates a new thread for handleClient for each accepted client.
 * @param[in] data should be set as NULL
 * @return
 */
void* handleConnectionRequest(void* data);
void* handleConnections(void* data);
void ServerSendDirectMessage(Client* client, char* header, char* message);
void ServerSendGlobalMessage(ClientList* client_list, char* header, char* message);
void ServerSendRejectMessage(int socket, char* reason);
/**
 * handles a request from a client, will check message header to determine actions [write more]
 * @param[in] receivedMessage message recieved from the client
 * @param[in] client pointer to client object
 */
void ProcessRequest(Message* receivedMessage, Client* client);
/**
 * initializes the server on a given socket, will begin accepting connections
 * @param[in] socket socket the server will listen on
 * @param[in] maxClients max number of clients that can be stored
 */
void initServer(int socket, int maxClients, char* password);
void* receiveCommands(void* data);
ServerData* getServerData();