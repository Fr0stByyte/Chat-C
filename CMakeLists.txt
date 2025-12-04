cmake_minimum_required(VERSION 3.28.3)
project(Chat_C C)

set(CMAKE_C_STANDARD 11)

add_executable(Chat_C
        shared/main.c
        Server/Server.c
        shared/Messages.c
        Client/Client.c
        Server/Requests.c
        headers/Client.h
        Server/Clients.c
        headers/Clients.h
        headers/Types.h
        headers/Messages.h
        headers/Server.h
        Server/Send.c
        Client/TextProcessing.c
)
