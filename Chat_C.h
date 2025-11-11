//
// Created by kroff on 11/3/2025.
//

#ifndef CHAT_C_CHAT_C_H
#define CHAT_C_CHAT_C_H
#include <netinet/in.h>

#endif //CHAT_C_CHAT_C_H


typedef struct {
    int clientFd;
    struct sockaddr_in clientAddr;
    char name[24];
    int color;
    int isAllowed;
} Client;

typedef struct {
    int size;
    int capacity;
    Client** clientBuffer;
    pthread_mutex_t mutexLock;
} ClientList;

typedef struct {
    uint32_t timeStamp;
    uint32_t senderLength;
    uint32_t recipientLength;
    uint32_t headerLength;
    uint32_t bodyLength;
    uint32_t color;

    uint8_t senderName[24];
    uint8_t recipientName[24];
    uint8_t header[24];
    uint8_t body[256];
} Message;
/**
 * creates a socket and connects it to a server, used to establish a connection
 * @param[in] ip ip address the client will connect too
 * @param[in] port port the client will connect to
 * @return returns a socket if a sucessful connection was made, else returns -1
 */
int createClientSocket(char ip[16], int port);

/**
 * initializes the client and sends data to the server requesting to join the chatroom
 * @param[in] socket socket given by createClientSocket
 * @param[in] nameLength length of the username
 * @param[in] clientName name the client will give to the server
 * @param[in] clientColor color the client will give to the server, displayed to other users
 */
void initClient(int socket, uint32_t nameLength, uint8_t clientName[], uint32_t clientColor);
/**
 * serializes a message object into an array, must be used for transmission over sockets
 * @param[in] message pointer to message that will be serialized
 * @param[out] data pointer to array that will hold the serialized data
 */
void Serialize(Message* message, uint8_t data[1024]);

/**
 * deserializes data from an array back into a message structure. used after data is recieved from a socket
 * @param[in] data pointer to array where serialized data is stored
 * @param[in] dataSize size of data that should be deserialized
 * @param[out] signal pointer for variable that will store errors
 * @return message struct with deserialized values
 */
Message Deserialize(uint8_t data[1024], ssize_t dataSize);

/**
 * creates a message struct storing the correctly formated parameters. allocated on the stack.
 * @param[in] timeStamp time stamp that tells when the message was sent
 * @param senderLength length of name of sender
 * @param recipientLength length of name of recipient
 * @param[in] headerLength size of the header array (be sure to use sizeof(), not strlen()
 * @param[in] bodyLength size of body array (be sure to use sizeof(), not strlen()
 * @param[in] header array that stores the header, should be null terminated
 * @param[in] body array that stores the body, should be null terminated
 * @return message struct contain properly formatted data from parameters
 */
Message createMessage(uint32_t timeStamp, uint32_t senderLength, uint32_t recipientLength, uint32_t headerLength, uint32_t bodyLength, uint32_t color, uint8_t sender[], uint8_t recipient[],  uint8_t header[], uint8_t body[]);

/**
 * clears a buffer, probs won't use tho
 * @param[in] buffer array storing data to be cleared
 * @param[in] size size of buffer
 */
void ClearBuffer(uint8_t buffer[1024], size_t size);

/**
 * called when server receives a message to the client with the purpose of it being broadcast globally
 * @param client client object that message was received from
 * @param message message struct conating message data client sent
 */
void ServerReceiveGlobalMessage(Client* client, ClientList* clientlist, Message* message);

/**
 *occurs when a client receives a message relayed to it from the server
 * @param client client from which the message originated from
 * @param message message data to be displayed
 */
void ClientReceiveGlobalMessage(Message* message);

/**
 * handles request sent to client to connect to a room
 * @param client client that wishes to connect
 * @param message connection data (text color, name)
 */
void ServerReceiveJoinRequest(Client* client, ClientList* clients, Message* message);
void ServerReceiveDisconnectRequest(Client* client, ClientList* client_list);
void ServerReceivePrivateMessage(Client* client, ClientList* client_list, Message* message);
void removeClientFromList(ClientList* client_list, Client* client);
/**
 * creates a complete socket from the given port, oskcet is bound and listening
 * @param[in] port port number the server should listen on
 * @returns server's socket
 */
int createServerSocket(int port);
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
void* handleConnections(void* data);
/**
 * Creates a client object on the heap
 * @param[in] clientFd clients socket
 * @param[in] clientAddress stores client data such as ip
 * @return pointer to newly created client object
 */
Client* CreateClient(int clientFd, struct sockaddr_in clientAddress);
/**
 * creates a client list object on the heap
 * @param[in] capacity max amount of clients the list can store
 * @return pointer to newly created client list
 */
ClientList* CreateClientList(int capacity);
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
void initServer(int socket, int maxClients);

/**
 * adds a client to the clientList, probably thread safe...
 * @param[in] clientList pointer to list of clients (should be a global variable for multi-thread acess
 * @param[in] client pointer to the client object which will be added
 */
void addClientToList(ClientList* clientList, Client* client);

