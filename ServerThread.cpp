#include <iostream>
#include <memory>

#include "ServerThread.h"

void RobotFactory::Init(int id, std::vector<ServerNode> peersList) {
    primary_id = -1;
    factory_id = id;
    last_index = -1;
    committed_index = -1;
    peers = std::move(peersList);
}

RobotInfo RobotFactory::CreateRegularRobot(CustomerRequest order, int engineer_id) {
	RobotInfo robot;
	robot.CopyOrder(order);
	robot.SetEngineerId(engineer_id);

    std::promise<RobotInfo> prom;
    std::future<RobotInfo> fut = prom.get_future();

    std::unique_ptr<AdminRequest> req = std::unique_ptr<AdminRequest>(new AdminRequest);
    req->robot = robot;
    req->prom = std::move(prom);

    arq_lock.lock();
    arq.push(std::move(req));
    arq_cv.notify_one();
    arq_lock.unlock();

    robot = fut.get();
	return robot;
}

void RobotFactory::EngineerThread(std::unique_ptr<ServerSocket> socket, int id) {
    //std::cout << "inside engineer thread" << std::endl;
	int engineer_id = id;
    ServerStub stub;
	stub.Init(std::move(socket));
    IdentificationMessage identificationMessage = stub.ReceiveIdentifyMessage();
    //std::cout << "Request type is: " << identificationMessage.GetType() << std::endl; //correct
    switch(identificationMessage.GetType()) {
        case 1:
            processReplicationRequest(stub);
            break;
        case 2:
            processCustomerRequest(engineer_id, stub);
            break;
        default:
            std::cout << "Undefined identification message tyep: " << identificationMessage.GetType() << std::endl;
    }
    //std::cout << "out of engineer thread "  << std::endl;

}

void RobotFactory::processReplicationRequest(ServerStub &stub) {
    ReplicationRequest request;
    ReplicationResponse response;
    while(true) {
        request = stub.ReceiveReplicationRequest();
        //request.Print();

        if(!request.isValid()) {
            //std::cout << "Invalid request: primary server is down "  << std::endl;
            primary_id = -1;
            //stub.CloseSocket();
            break;
        }
        int req_priId = request.getPrimaryId();
        int req_comIdx = request.getCommittedIndex();
        int req_lastIdx = request.getLastIndex();
        MapOp req_op = request.getLastOp();
        if(primary_id != req_priId) {
            primary_id = req_priId;
        }

        std::unique_lock<std::mutex> mlock(map_lock);
        // if this server is ahead of primary, rollback to the same status of primary.
        if(req_lastIdx < (int)smr_log.size()) {
            smr_log[req_lastIdx] = req_op;
        } else {
            smr_log.push_back(req_op);
        }
        last_index = req_lastIdx;
        //std::cout << "smr_log[req_lastIdx] = "  << smr_log[last_index].opcode << ", " << smr_log[last_index].arg1 << ", " << smr_log[last_index].arg2 << std::endl;
        int cusId = smr_log[req_comIdx].arg1;
        int ordNum = smr_log[req_comIdx].arg2;
        customer_record[cusId] = ordNum;
        mlock.unlock();
        committed_index = req_comIdx;

        response.SetStatus(1);
        if(!stub.SendReplicationResponse(response)) {
            primary_id = -1;
            //stub.CloseSocket();
            break;
        }
    }
}

void RobotFactory::processCustomerRequest(int engineer_id, ServerStub &stub) {
    RobotInfo robot;
    CustomerRequest request;
    int request_type;
    while (true) {
        request = stub.ReceiveRequest();
        if (!request.IsValid()) {
            break;
        }
        request_type = request.GetRequestType();
        switch (request_type) {
            case 1:
                robot = CreateRegularRobot(request, engineer_id);
                stub.SendRobot(robot);
                break;
            case 2:
                stub.SendRecord(GetCustomerRecord(request.GetCustomerId()));
                break;
            default:
                std::cout << "Undefined request type: "
                          << request_type << std::endl;

        }
    }
}

//PFA Thread
void RobotFactory::AdminThread(int id) {
	std::unique_lock<std::mutex> ul(arq_lock, std::defer_lock);
	while (true) {
		ul.lock();

		if (arq.empty()) {
			arq_cv.wait(ul, [this]{ return !arq.empty(); });
		}
        if(primary_id != factory_id) {
            BecomePrimaryNode();
        }
		auto adminReq = std::move(arq.front());
		arq.pop();

		ul.unlock();

		int cusId = adminReq->robot.GetCustomerId();
		int ordNum = adminReq->robot.GetOrderNumber();
		//std::cout <<"cusID: " << cusId << " , ordNum: " << ordNum << std::endl;
		//change smr_log
		smr_log.push_back({1, cusId, ordNum});
		last_index++;
		//std::cout << "before Replication req : last = " << last_index << "comIdx = " << committed_index << std::endl;
        //std::cout << "last Log after smr pushBack: "  <<  smr_log[last_index].opcode << ", " << smr_log[last_index].arg1 << ", " << smr_log[last_index].arg2 << std::endl;
		SendReplicationRequests();
		std::unique_lock<std::mutex> mlock(map_lock);
		//change customer_record
		customer_record[cusId] = ordNum;
		committed_index++;
		mlock.unlock();

        adminReq->robot.SetAdminId(id);
        adminReq->prom.set_value(adminReq->robot);
	}
}

CustomerRecord RobotFactory::GetCustomerRecord(int id) {
    CustomerRecord record;
    std::unique_lock<std::mutex> ul(map_lock);
    if (customer_record.find(id) != customer_record.end()) {
        record.SetRecord(id, customer_record[id]);
    }
    ul.unlock();
    return record;
}

void RobotFactory::BecomePrimaryNode() {
    primary_id = factory_id;
    for(ServerNode &node : peers) {
        node.stub = new ServerPFAStub();
        node.isActive = false;
        if(node.stub->Init(node.ip, node.port)) {
            if(node.stub->SendIdentifyAsPFA()) {
                node.isActive = true;
            }
        }
    }
    if(last_index != -1) {
        SendReplicationRequests(); //replicate current status, make sure everyone is on the same page.
        //make up for the lag with previous primary node
        std::unique_lock<std::mutex> mlock(map_lock);
        MapOp lastCommitedLog = smr_log[++committed_index];
        customer_record[lastCommitedLog.arg1] = lastCommitedLog.arg2;
        mlock.unlock();
    }
}

void RobotFactory::SendReplicationRequests() {
    for(ServerNode &node : peers) {
        if(!node.isActive) {
            RecoverNode(node);
            //std::cout << "recoveryResult: " << node.isActive << std::endl;
        }
        if(node.isActive) {
            SendSingleReplicationReq(primary_id,committed_index,last_index,smr_log[last_index],node);
        }
    }
}

void RobotFactory::SendSingleReplicationReq(int priId, int comIdx, int lastIdx, MapOp &op, ServerNode &node) {
    ReplicationRequest request;
    ReplicationResponse response;
    request.SetRequest(priId, comIdx, lastIdx, op);
    //std::cout << " Request to be sent: " << std::endl;
    //request.Print();
    response = node.stub->SendReplicationRequest(request);
    if(!response.GetStatus()) {
        //std::cout << " a node is down " << std::endl;
        node.isActive = false;
        node.stub->Close();
    }
}

void RobotFactory::RecoverNode(ServerNode &node) {
    if(!node.stub->Init(node.ip, node.port)) {
        return;
    }
    if(!node.stub->SendIdentifyAsPFA()) {
        return;
    }
    node.isActive = true;
    //std::cout << " node is back, start recovery " << std::endl;
    int temp = 0;
    while(temp <= last_index) {
        if(!node.isActive) {
            return;
        }
        SendSingleReplicationReq(primary_id,temp-1,temp,smr_log[temp], node);
        temp++;
    }
}



