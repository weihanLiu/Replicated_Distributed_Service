#ifndef __CLIENT_STUB_H__
#define __CLIENT_STUB_H__

#include <string>

#include "SenderSocket.h"
#include "Messages.h"

class ClientStub {
private:
	SenderSocket socket;
public:
	ClientStub();
	int Init(std::string ip, int port);
	RobotInfo Order(CustomerRequest request);
    CustomerRecord ReadRecord(CustomerRequest request);
    int SendIdentifyAsCustomer();

};


#endif // end of #ifndef __CLIENT_STUB_H__
