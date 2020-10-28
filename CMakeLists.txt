cmake_minimum_required(VERSION 3.16)
project(src)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

#add_executable(src
#        ClientMain.cpp
#        SenderSocket.cpp
#        SenderSocket.h
#        ClientStub.cpp
#        ClientStub.h
#        ClientThread.cpp
#        ClientThread.h
#        ClientTimer.cpp
#        ClientTimer.h
#        Messages.cpp
#        Messages.h
#        Socket.cpp
#        Socket.h)

add_executable(src
        Messages.cpp
        Messages.h
        ServerMain.cpp
        ServerSocket.cpp
        ServerSocket.h
        ServerStub.cpp
        ServerStub.h
        ServerThread.cpp
        ServerThread.h
        Socket.cpp
        Socket.h
        ServerPFAStub.cpp
        ServerPFAStub.h
        SenderSocket.cpp
        SenderSocket.h
        ServerMessages.h
        ServerMessages.cpp)

SET(CMAKE_CXX_FLAGS -pthread)
