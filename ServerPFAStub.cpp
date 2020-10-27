//
// Created by Weihan on 10/26/2020.
//

#include "ServerPFAStub.h"


ServerPFAStub::ServerPFAStub() {}

int ServerPFAStub::SendIdentifyAsPFA() {
    char buffer[32];
    IdentificationMessage message;
    message.SetType(1);
    message.Marshal(buffer);
    return socket.Send(buffer, message.Size(), 0);
}

int ServerPFAStub::Init(std::string ip, int port) {
    return socket.Init(ip, port);
}

ReplicationResponse ServerPFAStub::SendReplicationRequest(ReplicationRequest &request) {
    ReplicationResponse response;
    char buffer[32];
    ReplicationRequest request1;
    request1.SetRequest(0,-1,0,MapOp({1,1,1}));
    request1.Marshal(buffer);
    int size = request1.Size();
    if(socket.Send(buffer, size,0)) {
        size = response.Size();
        if(socket.Recv(buffer,size,0)) {
            response.Unmarshal(buffer);
        }
    }
    return response;
}