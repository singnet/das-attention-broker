#include "AttentionBrokerServer.h"

using namespace attention_broker_server;

// --------------------------------------------------------------------------------
// Public methods
  
AttentionBrokerServer::AttentionBrokerServer() {

    stimulus_requests = new RequestQueue(REQUEST_QUEUES_INTIAL_SIZE);
    correlation_requests = new RequestQueue(REQUEST_QUEUES_INTIAL_SIZE);
}

AttentionBrokerServer::~AttentionBrokerServer() {
    delete stimulus_requests;
    delete correlation_requests;
}

Status AttentionBrokerServer::ping(ServerContext* context, const das::Empty* request, das::Ack* reply) {
    reply->set_msg("PING");
    return Status::OK;
}

Status AttentionBrokerServer::stimulate(ServerContext* context, const das::HandleCount* request, das::Ack* reply) {
    stimulus_requests->enqueue((void *) request);
    reply->set_msg("STIMULATE");
    return Status::OK;
}

Status AttentionBrokerServer::correlate(ServerContext* context, const das::HandleList* request, das::Ack* reply) {
    correlation_requests->enqueue((void *) request);
    reply->set_msg("CORRELATE");
    return Status::OK;
}

// --------------------------------------------------------------------------------
// Private methods
