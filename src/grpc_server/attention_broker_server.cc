#include "attention_broker_server.h"

AttentionBrokerServer::AttentionBrokerServer() {
}

AttentionBrokerServer::~AttentionBrokerServer() {
}

Status AttentionBrokerServer::ping(ServerContext* context, const das::Empty* request, das::Ack* reply) {
    reply->set_msg("OK");
    return Status::OK;
}
