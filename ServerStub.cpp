#include "ServerStub.h"
#include <iostream>
ServerStub::ServerStub() {}

void ServerStub::Init(std::unique_ptr<ServerSocket> socket) {
	this->socket = std::move(socket);
}

CustomerRequest ServerStub::ReceiveRequest() {
	char buffer[32];
	CustomerRequest request;
	if (socket->Recv(buffer, request.Size(), 0)) {
		request.Unmarshal(buffer);
	}
	return request;
}

int ServerStub::SendRobot(RobotInfo info) {
	char buffer[32];
	info.Marshal(buffer);
	return socket->Send(buffer, info.Size(), 0);
}

int ServerStub::SendRecord(CustomerRecord record) {
    char buffer[32];
    record.Marshal(buffer);
    return socket->Send(buffer, record.Size(), 0);
}

IdentificationMessage ServerStub::ReceiveIdentifyMessage() {
    char buffer[32];
    IdentificationMessage message;
    if(socket->Recv(buffer, message.Size(), 0)) {
        message.Unmarshal(buffer);
    }
    return message;
}

ReplicationRequest ServerStub::ReceiveReplicationRequest() {
    char buffer[32];
    ReplicationRequest request;
    int temp = socket->Recv(buffer, request.Size(), 0);
    std::cout << "socket received result: "  << temp << std::endl;
    if(temp) {
        request.Unmarshal(buffer);
        std::cout << "Replication received: "  << std::endl;
        request.Print();
    }
    return request;
}

int ServerStub::SendReplicationResponse(ReplicationResponse response) {
    response.Print();
    char buffer[32];
    response.Marshal(buffer);
    return socket->Send(buffer, response.Size(),0);
}


