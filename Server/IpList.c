#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "../headers/Server.h"

IpList* createIpList(int capacity) {
    IpList* ipList = (IpList*)malloc(sizeof(IpList));
    ipList->capacity = capacity;
    ipList->length = 0;
    ipList->buffer = (char**)calloc(capacity, sizeof(char*));

    return ipList;
}

void appendIPList(IpList* ipList, struct in_addr* address) {
    char clientAddr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, address, clientAddr, INET_ADDRSTRLEN);

    if (ipList->length >= ipList->capacity) {
        realloc(ipList->buffer, ipList->capacity * 2);
        ipList->capacity *= 2;
    }
    ipList->buffer[ipList->length] = (char*)malloc(sizeof(char) * INET_ADDRSTRLEN);

    strncpy(ipList->buffer[ipList->length], clientAddr, INET_ADDRSTRLEN);
    ipList->length++;
}

void removefromIPList(IpList* ipList, struct in_addr* address) {
    char clientAddr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, address, clientAddr, INET_ADDRSTRLEN);

    int targetIndex = -1;
    for (int i = 0; i < ipList->length; i++) {
        if (strcmp(ipList->buffer[i], clientAddr) == 0) targetIndex = i;
    }
    if (targetIndex == -1) {
        printf("Client IP Address Not Found\n");
        return;
    }
    for (int i = targetIndex; i < ipList->length - 1; i++) {
        ipList->buffer[i] = ipList->buffer[i + 1];
    }
    ipList->length--;
}

int checkIPList(IpList* ipList, struct in_addr* address) {
    char clientAddr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, address, clientAddr, INET_ADDRSTRLEN);

    for (int i = 0; i < ipList->length; i++) {
        if (strcmp(ipList->buffer[i], clientAddr) == 0) {
            printf("found %s\n", clientAddr);
            return 0;
        }
    }
    return 1;
}
void destroyIPList(IpList* ipList) {
    free(ipList->buffer);
    free(ipList);
}