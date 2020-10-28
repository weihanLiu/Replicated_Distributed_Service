#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <string>
#include <map>

/**
 * Identification Message
 */
class IdentificationMessage {
private:
    //1 for replication request, 2 for customer request, 0 for invalid request
    int type;
public:
    IdentificationMessage();
    void operator = (const IdentificationMessage &message) {
        type = message.type;
    }

    int GetType();
    void SetType(int newType);
    void Unmarshal(char *buffer);
    void Marshal(char *buffer);
    int Size();
};

/**
 * Customer Request
 */
class CustomerRequest {
private:
	int customer_id;
	int order_number;
	int request_type;

public:
	CustomerRequest();
	void operator = (const CustomerRequest &request) {
		customer_id = request.customer_id;
		order_number = request.order_number;
		request_type = request.request_type;
	}
	void SetRequest(int cid, int order_num, int type);
	int GetCustomerId();
	int GetOrderNumber();
	int GetRequestType();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};


/**
 * Represent a Robot
 */
class RobotInfo {
private:
	int customer_id;
	int order_number;
	int request_type;
	int engineer_id;
	int admin_id;

public:
	RobotInfo();
	void operator = (const RobotInfo &info) {
		customer_id = info.customer_id;
		order_number = info.order_number;
		request_type = info.request_type;
		engineer_id = info.engineer_id;
		admin_id = info.admin_id;
	}
	void SetInfo(int cid, int order_num, int type, int engid, int expid);
	void CopyOrder(CustomerRequest request);
	void SetEngineerId(int id);
	void SetAdminId(int id);

	int GetCustomerId();
	int GetOrderNumber();
	int GetRobotType();
	int GetEngineerId();
	int GetAdminId();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};

/**
 * Represent a Customer Record
 */
class CustomerRecord {
private:
    int customer_id; // copied from the read request; -1 if customer_id is not found in the map
    int last_order; // copied from the map; -1 if customer_id is not found in the map
public:
    CustomerRecord();
    void operator=(const CustomerRecord &record) {
        this->customer_id = record.customer_id;
        this->last_order = record.last_order;
    }

    int Size();

    void Marshal(char *buffer);
    void Unmarshal(char *buffer);

    void SetRecord(int id, int last_order);

    int GetCustomerId() const;
    int GetLastOrder() const;

    void SetCustomerId(int customerId);
    void SetLastOrder(int lastOrder);

    void Print() const;
    bool IsValid();
};

#endif // #ifndef __MESSAGES_H__
