//
// Created by Weihan on 10/27/2020.
//

#include <arpa/inet.h>
#include <cstring>
#include <iostream>

#include "ServerMessages.h"


ReplicationRequest::ReplicationRequest() {
    primary_id = -1;
    committed_index = -1;
    last_index = -1;
    opcode = -1;
    arg1 = -1;
    arg2 = -1;
}

bool ReplicationRequest::isValid() {
    return primary_id != -1 && last_index != -1;
}

int ReplicationRequest::getPrimaryId() {
    return primary_id;
}

int ReplicationRequest::getCommittedIndex() {
    return committed_index;
}

int ReplicationRequest::getLastIndex() {
    return last_index;
}

MapOp ReplicationRequest::getLastOp() {
    return MapOp({opcode,arg1,arg2});
}

void ReplicationRequest::SetRequest(int priId, int comIdx, int lastIdx, MapOp &op) {
    primary_id = priId;
    committed_index = comIdx;
    last_index = lastIdx;
    opcode = op.opcode;
    arg1 = op.arg1;
    arg2 = op.arg2;
    //std::cout << "mapOp in setRequest = "  << opcode << ", " << arg1 << ", " << arg2 << std::endl;
}

int ReplicationRequest::Size() {
    return sizeof(primary_id) + sizeof(committed_index) + sizeof(last_index) +
            sizeof(opcode) + sizeof(arg1) + sizeof(arg2);
}

void ReplicationRequest::Marshal(char *buffer) {
    int net_primary_id = htonl(primary_id);
    int net_committed_index = htonl(committed_index);
    int net_last_index = htonl(last_index);
    int net_opcode = htonl(opcode);
    int net_arg1 = htonl(arg1);
    int net_arg2 = htonl(arg2);

    int offset = 0;
    memcpy(buffer + offset, &net_primary_id, sizeof(net_primary_id));
    offset += sizeof(net_primary_id);
    memcpy(buffer + offset, &net_committed_index, sizeof(net_committed_index));
    offset += sizeof(net_committed_index);
    memcpy(buffer + offset, &net_last_index, sizeof(net_last_index));
    offset += sizeof(net_last_index);
    memcpy(buffer + offset, &net_opcode, sizeof(net_opcode));
    offset += sizeof(net_opcode);
    memcpy(buffer + offset, &net_arg1, sizeof(net_arg1));
    offset += sizeof(net_arg1);
    memcpy(buffer + offset, &net_arg2, sizeof(net_arg2));
}

void ReplicationRequest::Unmarshal(char *buffer) {
    int net_primary_id;
    int net_committed_index;
    int net_last_index;
    int net_opcode;
    int net_arg1;
    int net_arg2;

    int offset = 0;
    memcpy(&net_primary_id, buffer + offset, sizeof(net_primary_id));
    offset += sizeof(net_primary_id);
    memcpy(&net_committed_index, buffer + offset, sizeof(net_committed_index));
    offset += sizeof(net_committed_index);
    memcpy(&net_last_index, buffer + offset, sizeof(net_last_index));
    offset += sizeof(net_last_index);
    memcpy(&net_opcode, buffer + offset, sizeof(net_opcode));
    offset += sizeof(net_opcode);
    memcpy(&net_arg1, buffer + offset, sizeof(net_arg1));
    offset += sizeof(net_arg1);
    memcpy(&net_arg2, buffer + offset, sizeof(net_arg2));

    primary_id = ntohl(net_primary_id);
    committed_index = ntohl(net_committed_index);
    last_index = ntohl(net_last_index);
    opcode = ntohl(net_opcode);
    arg1 = ntohl(net_arg1);
    arg2 = ntohl(net_arg2);
}

void ReplicationRequest::Print() {
    std::cout << "priId " << this->primary_id << " ";
    std::cout << "comIdx " << this->committed_index << " ";
    std::cout << "lastInd " << this->last_index << " ";
    std::cout << "Mapop " << this->opcode << ", " << this->arg1 << ", " << this->arg2 << std::endl;

}

ReplicationResponse::ReplicationResponse() {
    status = -1;
}

int ReplicationResponse::GetStatus() const {
    return status;
}

void ReplicationResponse::SetStatus(int s) {
    status = s;
}

void ReplicationResponse::Marshal(char *buffer) {
    int net_status = htonl(this->status);
    memcpy(buffer, &net_status, sizeof(net_status));
}

void ReplicationResponse::Unmarshal(char *buffer) {
    int net_status;
    memcpy(&net_status, buffer, sizeof(net_status));
    status = ntohl(net_status);
}

int ReplicationResponse::Size() {
    return sizeof(status);
}

void ReplicationResponse::Print() {
    std::cout << "status " << this->status << std::endl;
}
