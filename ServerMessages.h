//
// Created by Weihan on 10/27/2020.
//

#ifndef SRC_SERVERMESSAGES_H
#define SRC_SERVERMESSAGES_H

#include "Messages.h"


struct MapOp {
    int opcode; // operation code: 1 - update value
    int arg1; // customer_id to apply the operation
    int arg2; // parameter for the operation
};

/**
 * Replication Request
 */
class ReplicationRequest {
private:
    int primary_id{};
    int committed_index{};
    int last_index{};
    int opcode{};
    int arg1{};
    int arg2{};
public:
    ReplicationRequest();
    void operator = (const ReplicationRequest &request) {
        primary_id = request.primary_id;
        committed_index = request.committed_index;
        last_index = request.last_index;
        opcode = request.opcode;
        arg1 = request.arg1;
        arg2 = request.arg2;
    }
    void SetRequest(int priId, int comIdx, int lastIdx, MapOp &op);

    void Marshal(char *buffer);
    void Unmarshal(char *buffer);
    int Size();

    int getPrimaryId();
    int getCommittedIndex();
    int getLastIndex();
    MapOp getLastOp();
    bool isValid();
    void Print();
};

class ReplicationResponse {
private:
    //status = 1, replication success; status = 0, failed
    int status;
public:
    void operator = (const ReplicationResponse &response) {
        status = response.status;
    }
    ReplicationResponse();
    int GetStatus() const;
    void SetStatus(int status);
    int Size();
    void Marshal(char *buffer);
    void Unmarshal(char *buffer);
    void Print();

};


#endif //SRC_SERVERMESSAGES_H
