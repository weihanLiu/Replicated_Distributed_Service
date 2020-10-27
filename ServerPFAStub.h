//
// Created by Weihan on 10/26/2020.
//

#ifndef SRC_SERVERPFASTUB_H
#define SRC_SERVERPFASTUB_H

#include <memory>
#include "SenderSocket.h"
#include "ServerMessages.h"


class ServerPFAStub {
private:
    SenderSocket socket;
public:
    ServerPFAStub();
    int Init(std::string ip, int port);
    int SendIdentifyAsPFA();
    ReplicationResponse SendReplicationRequest(ReplicationRequest &request);
};


#endif //SRC_SERVERPFASTUB_H
