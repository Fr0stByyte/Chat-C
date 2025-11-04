cmake_minimum_required(VERSION 3.28.3)
project(Chat_C C)

set(CMAKE_C_STANDARD 11)

add_executable(Chat_C
        main.c
        Server.c
        Messages.c
        Client.c
        Requests.c
        Chat_C.h
)
