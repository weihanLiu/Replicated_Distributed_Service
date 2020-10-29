//
// Created by Weihan on 10/26/2020.
//
#include <iostream>
#include "ServerPFAStub.h"


ServerPFAStub::ServerPFAStub() {}

int ServerPFAStub::SendIdentifyAsPFA() {
    char buffer[32];
    IdentificationMessage message;
    message.SetType(1);
    message.Marshal(buffer);
    if(socket.Send(buffer, message.Size(), 0) <= 0) {
        return 0;
    } else {
        return 1;
    }
}

int ServerPFAStub::Init(std::string ip, int port) {
    int temp = socket.Init(ip, port);
    if(temp == 0) {
        socket.Close();
    }
    //std::cout << " Inside sockect init: " << temp << std::endl;
    return temp;
}

ReplicationResponse ServerPFAStub::SendReplicationRequest(ReplicationRequest &request) {
    ReplicationResponse response;
    char buffer[32];
    request.Marshal(buffer);
    int size = request.Size();
    response.SetStatus(0);
    if(socket.Send(buffer, size,0) > 0) {
        size = response.Size();
        if(socket.Recv(buffer,size,0) > 0) {
            response.Unmarshal(buffer);
        }
    }
    return response;
}

void ServerPFAStub::Close() {
    socket.Close();
}