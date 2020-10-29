#ifndef __SERVERTHREAD_H__
#define __SERVERTHREAD_H__

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "Messages.h"
#include "ServerMessages.h"
#include "ServerSocket.h"
#include "ServerStub.h"
#include "ServerPFAStub.h"


struct ServerNode {
    int id;
    std::string ip;
    int port;
    ServerPFAStub* stub;
    bool isActive;
};

struct AdminRequest {
    RobotInfo robot;
    std::promise<RobotInfo> prom;
};

class RobotFactory {
private:
    int primary_id;
    int factory_id;
    int last_index;
    int committed_index;
    std::vector<ServerNode> peers;

	std::queue<std::unique_ptr<AdminRequest>> arq;
	std::mutex arq_lock, map_lock;
	std::condition_variable arq_cv;

    std::map <int,int > customer_record;
    std::vector <MapOp > smr_log; // state machine replication log

	RobotInfo CreateRegularRobot(CustomerRequest request, int engineer_id);
	CustomerRecord GetCustomerRecord(int cusId);
    void BecomePrimaryNode();
    void SendReplicationRequests();
    void processCustomerRequest(int eng_id, ServerStub &stub);
    void processReplicationRequest(ServerStub &stub);
    void RecoverNode(ServerNode &node);
    void SendSingleReplicationReq(int priId, int comIdx, int lastIdx, MapOp &op, ServerNode &node);

//	RobotInfo CreateSpecialRobot(RobotOrder order, int engineer_id);
public:
    void Init(int id, std::vector<ServerNode> peers);
	void EngineerThread(std::unique_ptr<ServerSocket> socket, int id);
	void AdminThread(int id);
};



#endif // end of #ifndef __SERVERTHREAD_H__

