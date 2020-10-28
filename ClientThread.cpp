#include "ClientThread.h"
#include "Messages.h"

#include <iostream>
#include <utility>

ClientThreadClass::ClientThreadClass() {}

void ClientThreadClass::ThreadBody(std::string ip, int port, int id, int orders, int type) {
	customer_id = id;
	num_orders = orders;
	request_type = type;
	if (!stub.Init(std::move(ip), port)) {
		//std::cout << "Thread " << customer_id << " failed to connect" << std::endl;
		return;
	}
	if(!stub.SendIdentifyAsCustomer()) {
        //std::cout << "Thread " << customer_id << " failed to send identify message." << std::endl;
        return;
	}
    CustomerRequest request;
    CustomerRecord record;
	switch(request_type) {
	    case 1:
            for (int i = 0; i < num_orders; i++) {
                RobotInfo robot;
                request.SetRequest(customer_id, i, request_type);

                timer.Start();
                robot = stub.Order(request);
                timer.EndAndMerge();

                if (!robot.IsValid()) {
                    //std::cout << "Fail to send request or receive from server, id: " << customer_id << std::endl;
                    return;
                }
            }
            break;
	    case 2:
	        request.SetRequest(customer_id, -1, request_type);

	        timer.Start();
	        record = stub.ReadRecord(request);
	        if(record.GetCustomerId() == -2 && record.GetLastOrder() == -2) {
                //std::cout << "Fail to send request or received from server" << std::endl;
                return;
	        }
	        timer.EndAndMerge();
	        break;
	    case 3:
	        for (int i = 0; i <= num_orders; i++) {
	            request.SetRequest(i, -1, 2);

	            timer.Start();
	            record = stub.ReadRecord(request);
                if(record.GetCustomerId() == -2 && record.GetLastOrder() == -2) {
                    //std::cout << "Fail to send request or received from server" << std::endl;
                    return;
                }
	            timer.EndAndMerge();

	            if (record.GetCustomerId() != -1) {
	                record.Print();
	            }
	        }
	        break;
	    default:
	        std::cout<< "Invalid request type: " << request_type <<std::endl;
	}
}

ClientTimer ClientThreadClass::GetTimer() {
	return timer;	
}

