#ifndef __SERVER_STUB_H__
#define __SERVER_STUB_H__

#include <memory>

#include "ServerSocket.h"
#include "Messages.h"
#include "ServerMessages.h"

class ServerStub {
private:
	std::unique_ptr<ServerSocket> socket;
public:
	ServerStub();
	void Init(std::unique_ptr<ServerSocket> socket);
	CustomerRequest ReceiveRequest();
	int SendRobot(RobotInfo info);
	int SendRecord(CustomerRecord record);
    int SendReplicationResponse(ReplicationResponse response);

    IdentificationMessage ReceiveIdentifyMessage();
    ReplicationRequest ReceiveReplicationRequest();

};

#endif // end of #ifndef __SERVER_STUB_H__
