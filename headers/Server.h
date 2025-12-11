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
 * @param client_list client list to be read from
 * @param message message struct conating message data client sent
 */
void ServerReceiveGlobalMessage(Client* client, ClientList* client_list, Message* message);
/**
 * handles request sent to client to connect to a room
 * @param socket client socket that wishes to connect
 * @param clientReturn returned client object
 * @param message connection data (text color, name)
 * @param ipAddress ip of client
 */
int ServerReceiveJoinRequest(int socket, Message* message, Client** clientReturn, struct sockaddr_in* ipAddress);

/**
 * handles client disconnect requests
 * @param client client disconnecting
 * @param client_list list to be read from
 */
void ServerReceiveDisconnectRequest(Client* client, ClientList* client_list);

/**
 * handles private message requests and relays them to target
 * @param client client sending message
 * @param client_list list to be read from
 * @param message message to be sent
 */
void ServerReceivePrivateMessage(Client* client, ClientList* client_list, Message* message);
/**
 * creates a complete socket from the given port, oskcet is bound and listening
 * @returns server's socket
 */
int createServerSocket();
/**
 * handles data coming in from a client, should be on its own thread.
 * @param[in] data pointer to a client object
 * @return
 */
void* handleClient(void* data);
/**
 * determines if a client can join, creates a client, then defers to handleClient
 * @param[in] data pointer to connectionData
 * @return
 */
void* handleConnectionRequest(void* data);

/**
 * handles incoming connections and defers them to handleConnectionRequest
 * @param data server data
 * @return
 */
void* handleConnections(void* data);

/**
 * sends a direct message to target client
 * @param client client to be messaged
 * @param message message text
 */
void ServerSendDirectMessage(Client* client, char* message);

/**
 * sends a message to all clients in client list
 * @param client_list list of clients
 * @param message message text to be sent
 */
void ServerSendGlobalMessage(ClientList* client_list, char* message);

/**
 * sends a message explaining why the server refused a join
 * @param socket socket to be messaged
 * @param reason reason for refusing connection
 */
void ServerSendRejectMessage(int socket, char* reason);

/**
 * handles player information requests. Tells client number of users and their names
 * @param client client sending request
 */
void ServerReceivePlayersRequest(Client* client);

/**
 * handles requests to change message color
 * @param client client requesting color change
 * @param color color to be changed to
 */
void ServerReceiveColorRequest(Client* client, int color);
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
 * @param[in] password join password to be set
 */
void initServer(int socket, int maxClients, char* password);

/**
 * receives server commands and processes them
 * @param data ServerData
 * @return
 */
void* receiveCommands(void* data);

/**
 * returns server data, used by other files to get important server information in scope
 * @return the server data
 */
ServerData* getServerData();

/**
 * creates a list that can store client ip addresses
 * @param capacity max amount of Ips that can be stored
 * @return new list where ips can be stored
 */
IpList* createIpList(int capacity);

/**
 * adds a new ip to iplist
 * @param ipList list to be accessed
 * @param address ip address to be added
 */
void appendIPList(IpList* ipList, struct in_addr* address);

/**
 * removes an ip from the ip list
 * @param ipList list to be accessed
 * @param address address to be added
 */
void removefromIPList(IpList* ipList, struct in_addr* address);

/**
 * checks if an ip is in a list, returns and int based on that
 * @param ipList list to be accessed
 * @param address address to check for
 * @return 0 if found, 1 if not found
 */
int checkIPList(IpList* ipList, struct in_addr* address);

/**
 * properly frees up memory and deletes an ip list
 * @param ipList list to be destroyed
 */
void destroyIPList(IpList* ipList);