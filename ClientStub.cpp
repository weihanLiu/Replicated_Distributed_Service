#include "ClientStub.h"

ClientStub::ClientStub() {}

int ClientStub::Init(std::string ip, int port) {
	return socket.Init(ip, port);	
}

RobotInfo ClientStub::Order(CustomerRequest order) {
	RobotInfo info;
	char buffer[32];
	int size;
	order.Marshal(buffer);
	size = order.Size();
	if (socket.Send(buffer, size, 0)) {
		size = info.Size();
		if (socket.Recv(buffer, size, 0)) {
			info.Unmarshal(buffer);
		} 
	}
	return info;
}

CustomerRecord ClientStub::ReadRecord(CustomerRequest request) {
    CustomerRecord record;
    char buffer[32];
    int size;
    request.Marshal(buffer);
    size = request.Size();
    record.SetRecord(-2, -2);
    if (socket.Send(buffer, size, 0)) {
        size = record.Size();
        if (socket.Recv(buffer, size, 0)) {
            record.Unmarshal(buffer);
        }
    }
    return record;
}

int ClientStub::SendIdentifyAsCustomer() {
    char buffer[32];
    IdentificationMessage message;
    message.SetType(2);
    message.Marshal(buffer);
    return socket.Send(buffer, message.Size(), 0);
}

void ClientStub::CloseSocket() {
    socket.Close();
}

