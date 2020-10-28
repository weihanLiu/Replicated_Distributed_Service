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
	int engineer_id = id;
    ServerStub stub;
	stub.Init(std::move(socket));
    IdentificationMessage identificationMessage = stub.ReceiveIdentifyMessage();
    std::cout << "Request type is: " << identificationMessage.GetType() << std::endl; //correct
    switch(identificationMessage.GetType()) {
        case 1:
            std::cout << "first stub" << &stub << std::endl;
            processReplicationRequest(stub);
            break;
        case 2:
            processCustomerRequest(engineer_id, stub);
            break;
        default:
            std::cout << "Undefined identification message tyep: " << identificationMessage.GetType() << std::endl;
    }
}

void RobotFactory::processReplicationRequest(ServerStub &stub) {
    ReplicationRequest request;
    ReplicationResponse response;
    while(true) {
        std::cout << "second stub" << &stub << std::endl;
        request = stub.ReceiveReplicationRequest();
        request.Print();

        if(!request.isValid()) {
            std::cout << "Invalid request: "  << std::endl;
            response.SetStatus(0);
            stub.SendReplicationResponse(response);
            break;
        }
        int req_priId = request.getPrimaryId();
        int req_comIdx = request.getCommittedIndex();
        int req_lastIdx = request.getLastIndex();
        MapOp req_op = request.getLastOp();
        if(primary_id != req_priId) {
            primary_id = req_priId;
        }
        smr_log.push_back( req_op);
        last_index = req_lastIdx;
        std::cout << "smr_log[req_lastIdx] = "  << smr_log[last_index].opcode << ", " << smr_log[last_index].arg1 << ", " << smr_log[last_index].arg2 << std::endl;

        if(req_comIdx != -1 && committed_index != req_comIdx) {
            std::unique_lock<std::mutex> mlock(map_lock);
            while(committed_index < req_comIdx) {
                committed_index++;
                int cusId = smr_log[committed_index].arg1;
                int ordNum = smr_log[committed_index].arg2;
                customer_record[cusId] = ordNum;
            }
            mlock.unlock();
        }
        response.SetStatus(1);
        stub.SendReplicationResponse(response);
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
		std::cout <<"cusID: " << cusId << " , ordNum: " << ordNum << std::endl;
		//change smr_log
		smr_log.push_back({1, cusId, ordNum});
		last_index++;
        std::cout << "last Log after smr pushBack: "  <<  smr_log[last_index].opcode << ", " << smr_log[last_index].arg1 << ", " << smr_log[last_index].arg2 << std::endl;
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
    if(primary_id != -1) {
        //committe the xth record in the log and change committed_index to x.
        MapOp xthOpLog = smr_log[++committed_index];
        std::unique_lock<std::mutex> mlock(map_lock);
        customer_record[xthOpLog.arg1] = xthOpLog.arg2;
        mlock.unlock();
    }
    primary_id = factory_id;
    for(ServerNode &node : peers) {
        std::cout << "current node is: " << &node << std::endl;
        node.stub = new ServerPFAStub();
        node.stub->Init(node.ip, node.port);
        node.stub->SendIdentifyAsPFA(); //Send by PFAStub which use client socket
    }
}

void RobotFactory::SendReplicationRequests() {
    for(ServerNode &node : peers) {
        ReplicationRequest request;
        ReplicationResponse response;
        request.SetRequest(primary_id,committed_index,last_index, smr_log[last_index]);
        std::cout << " Request to be sent: " << std::endl;
        request.Print();
        response = node.stub->SendReplicationRequest(request);
        std::cout << " Response status: "  << response.GetStatus() << std::endl;
    }
    std::cout << "Send replication request completed "  << std::endl;
}



