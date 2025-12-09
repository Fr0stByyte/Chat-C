#include "../headers/Messages.h"
#include "../headers/Client.h"
void ClientSendGlobalMessage(char* message, int color, int clientSocket) {

    char recipient[] = "ALL";
    char header[] = "SEND GLOBAL";
    Message msg = createMessage(
        time(NULL),
        color,
        "",
        recipient,
        header,
        message
    );
    uint8_t buffer[1024];
    Serialize(&msg, buffer);
    send(clientSocket, buffer, sizeof(buffer), 0);
}

void ClientSendPrivateMessage(char* message, char* receiver, int color, int clientSocket) {
    char privateHeader[] = "SEND PRIVATE";
    Message privateMessage = createMessage(
        time(NULL),
        color,
    "",
    receiver,
        privateHeader,
    message
    );
    uint8_t buffer[1024];
    Serialize(&privateMessage, buffer);
    send(clientSocket, buffer, sizeof(buffer), 0);
}

void ClientSendDataRequest(char* data, int clientSocket) {
    char header[] = "REQUEST DATA";
    Message message = createMessage(
        time(NULL),
        0,
        "",
        "SERVER",
        header,
        data
    );
    uint8_t buffer[1024];
    Serialize(&message, buffer);
    send(clientSocket, buffer, sizeof(buffer), 0);
}