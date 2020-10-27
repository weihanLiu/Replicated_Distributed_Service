#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include "Messages.h"

IdentificationMessage::IdentificationMessage() {
    type = -1;
}

int IdentificationMessage::GetType() { return type; }

void IdentificationMessage::SetType(int newType) {
    type = newType;
}

void IdentificationMessage::Unmarshal(char *buffer) {
    int net_type;
    memcpy(&net_type, buffer, sizeof(net_type));
    type = ntohl(net_type);
}

void IdentificationMessage::Marshal(char* buffer) {
    int net_type = htonl(type);
    memcpy(buffer, &net_type, sizeof(net_type));
}

int IdentificationMessage::Size() {
    return sizeof(type);
}

CustomerRequest::CustomerRequest() {
	customer_id = -1;
	order_number = -1;
	request_type = -1;
}

void CustomerRequest::SetRequest(int id, int number, int type) {
	customer_id = id;
	order_number = number;
	request_type = type;
}

int CustomerRequest::GetCustomerId() { return customer_id; }
int CustomerRequest::GetOrderNumber() { return order_number; }
int CustomerRequest::GetRequestType() { return request_type; }

int CustomerRequest::Size() {
	return sizeof(customer_id) + sizeof(order_number) + sizeof(request_type);
}

void CustomerRequest::Marshal(char *buffer) {
	int net_customer_id = htonl(customer_id);
	int net_order_number = htonl(order_number);
	int net_request_type = htonl(request_type);
	int offset = 0;
	memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(buffer + offset, &net_order_number, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(buffer + offset, &net_request_type, sizeof(net_request_type));
}

void CustomerRequest::Unmarshal(char *buffer) {
	int net_customer_id;
	int net_order_number;
	int net_request_type;
	int offset = 0;
	memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(&net_order_number, buffer + offset, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(&net_request_type, buffer + offset, sizeof(net_request_type));

	customer_id = ntohl(net_customer_id);
	order_number = ntohl(net_order_number);
	request_type = ntohl(net_request_type);
}

bool CustomerRequest::IsValid() {
	return (customer_id != -1);
}

void CustomerRequest::Print() {
	std::cout << "id " << customer_id << " ";
	std::cout << "num " << order_number << " ";
	std::cout << "type " << request_type << std::endl;
}

RobotInfo::RobotInfo() {
	customer_id = -1;
	order_number = -1;
	request_type = -1;
	engineer_id = -1;
    admin_id = -1;
}

void RobotInfo::SetInfo(int id, int number, int type, int engid, int admid) {
	customer_id = id;
	order_number = number;
	request_type = type;
	engineer_id = engid;
    admin_id = admid;
}

void RobotInfo::CopyOrder(CustomerRequest order) {
	customer_id = order.GetCustomerId();
	order_number = order.GetOrderNumber();
	request_type = order.GetRequestType();
}
void RobotInfo::SetEngineerId(int id) { engineer_id = id; }
void RobotInfo::SetAdminId(int id) { admin_id = id; }

int RobotInfo::GetCustomerId() { return customer_id; }
int RobotInfo::GetOrderNumber() { return order_number; }
int RobotInfo::GetRobotType() { return request_type; }
int RobotInfo::GetEngineerId() { return engineer_id; }
int RobotInfo::GetAdminId() { return admin_id; }

int RobotInfo::Size() {
	return sizeof(customer_id) + sizeof(order_number) + sizeof(request_type)
		+ sizeof(engineer_id) + sizeof(admin_id);
}

void RobotInfo::Marshal(char *buffer) {
	int net_customer_id = htonl(customer_id);
	int net_order_number = htonl(order_number);
	int net_request_type = htonl(request_type);
	int net_engineer_id = htonl(engineer_id);
	int net_expert_id = htonl(admin_id);
	int offset = 0;

	memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(buffer + offset, &net_order_number, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(buffer + offset, &net_request_type, sizeof(net_request_type));
	offset += sizeof(net_request_type);
	memcpy(buffer + offset, &net_engineer_id, sizeof(net_engineer_id));
	offset += sizeof(net_engineer_id);
	memcpy(buffer + offset, &net_expert_id, sizeof(net_expert_id));

}

void RobotInfo::Unmarshal(char *buffer) {
	int net_customer_id;
	int net_order_number;
	int net_request_type;
	int net_engineer_id;
	int net_expert_id;
	int offset = 0;

	memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(&net_order_number, buffer + offset, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(&net_request_type, buffer + offset, sizeof(net_request_type));
	offset += sizeof(net_request_type);
	memcpy(&net_engineer_id, buffer + offset, sizeof(net_engineer_id));
	offset += sizeof(net_engineer_id);
	memcpy(&net_expert_id, buffer + offset, sizeof(net_expert_id));

	customer_id = ntohl(net_customer_id);
	order_number = ntohl(net_order_number);
	request_type = ntohl(net_request_type);
	engineer_id = ntohl(net_engineer_id);
	admin_id = ntohl(net_expert_id);
}

bool RobotInfo::IsValid() {
	return (customer_id != -1);
}

void RobotInfo::Print() {
	std::cout << "cus_id " << customer_id << " ";
	std::cout << "order_num " << order_number << " ";
	std::cout << "req_type " << request_type << " ";
	std::cout << "engid " << engineer_id << " ";
	std::cout << "admid " << admin_id << std::endl;
}

CustomerRecord::CustomerRecord() {
    this->customer_id = -1;
    this->last_order = -1;
}

void CustomerRecord::SetRecord(int id, int last) {
    this->customer_id = id;
    this->last_order = last;
}

int CustomerRecord::GetCustomerId() const {
    return customer_id;
}

int CustomerRecord::GetLastOrder() const {
    return last_order;
}

void CustomerRecord::SetCustomerId(int customerId) {
    customer_id = customerId;
}

void CustomerRecord::SetLastOrder(int lastOrder) {
    last_order = lastOrder;
}

int CustomerRecord::Size() {
    return sizeof(this->customer_id) + sizeof(this->last_order);
}

void CustomerRecord::Marshal(char *buffer) {
    int net_customer_id = htonl(this->customer_id);
    int net_last_order = htonl(this->last_order);
    int offset = 0;
    memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
    offset += sizeof(net_customer_id);
    memcpy(buffer + offset, &net_last_order, sizeof(net_last_order));
}

void CustomerRecord::Unmarshal(char *buffer) {
    int net_customer_id;
    int net_last_order;
    int offset = 0;

    memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
    offset += sizeof(net_customer_id);
    memcpy(&net_last_order, buffer + offset, sizeof(net_last_order));
    this->customer_id = ntohl(net_customer_id);
    this->last_order = ntohl(net_last_order);
}

void CustomerRecord::Print() const {
    std::cout << "cusId " << this->customer_id << " ";
    std::cout << "lastOrder " << this->last_order << std::endl;
}
