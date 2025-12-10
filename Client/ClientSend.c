#include "../headers/Messages.h"
#include "../headers/Client.h"
void ClientSendGlobalMessage(char* message, int clientSocket) {

    char recipient[] = "ALL";
    char header[] = "SEND GLOBAL";
    Message msg = createMessage(
        time(NULL),
        0,
        "",
        recipient,
        header,
        message
    );
    uint8_t buffer[1024];
    Serialize(&msg, buffer);
    send(clientSocket, buffer, sizeof(buffer), 0);
}

void ClientSendPrivateMessage(char* message, char* receiver, int clientSocket) {
    char privateHeader[] = "SEND PRIVATE";
    Message privateMessage = createMessage(
        time(NULL),
        0,
    "",
    receiver,
        privateHeader,
    message
    );
    uint8_t buffer[1024];
    Serialize(&privateMessage, buffer);
    send(clientSocket, buffer, sizeof(buffer), 0);
}

void ClientGetPlayersRequest(int clientSocket) {
    char header[] = "REQUEST PLAYERS";
    Message message = createMessage(
        time(NULL),
        0,
        "",
        "SERVER",
        header,
        ""
    );
    uint8_t buffer[1024];
    Serialize(&message, buffer);
    send(clientSocket, buffer, sizeof(buffer), 0);
}

void ClientColorRequest(int clientSocket, int color) {
    char header[] = "REQUEST COLOR";
    Message message = createMessage(
        time(NULL),
        color - 1,
        "",
        "SERVER",
        header,
        ""
    );
    uint8_t buffer[1024];
    Serialize(&message, buffer);
    send(clientSocket, buffer, sizeof(buffer), 0);
}