#ifndef __CLIENTSOCKET_H__
#define __CLIENTSOCKET_H__

#include <string>

#include "Socket.h"


class SenderSocket: public Socket {
public:
	SenderSocket() {}
	~SenderSocket() {}

	int Init(std::string ip, int port);
};


#endif // end of #ifndef __CLIENTSOCKET_H__
