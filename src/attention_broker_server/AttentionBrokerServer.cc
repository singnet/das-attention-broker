#include "AttentionBrokerServer.h"

using namespace attention_broker_server;

// --------------------------------------------------------------------------------
// Public methods
  
AttentionBrokerServer::AttentionBrokerServer() {
    stimulus_requests = new RequestQueue(REQUEST_QUEUE_INTIAL_SIZE);
    correlation_requests = new RequestQueue(REQUEST_QUEUE_INTIAL_SIZE);
    worker_threads = new WorkerThreadPool(stimulus_requests, correlation_requests);
}

AttentionBrokerServer::~AttentionBrokerServer() {
    graceful_shutdown();
    delete stimulus_requests;
    delete correlation_requests;
    delete worker_threads;
}

void AttentionBrokerServer::graceful_shutdown() {
    rpc_api_enabled = false;
    worker_threads->graceful_stop();
}

// RPC API

Status AttentionBrokerServer::ping(ServerContext* context, const das::Empty* request, das::Ack* reply) {
    reply->set_msg("PING");
    if (rpc_api_enabled) {
        return Status::OK;
    } else{
        return Status::CANCELLED;
    }
}

Status AttentionBrokerServer::stimulate(ServerContext* context, const das::HandleCount* request, das::Ack* reply) {
    stimulus_requests->enqueue((void *) request);
    reply->set_msg("STIMULATE");
    if (rpc_api_enabled) {
        return Status::OK;
    } else{
        return Status::CANCELLED;
    }
}

Status AttentionBrokerServer::correlate(ServerContext* context, const das::HandleList* request, das::Ack* reply) {
    correlation_requests->enqueue((void *) request);
    reply->set_msg("CORRELATE");
    if (rpc_api_enabled) {
        return Status::OK;
    } else{
        return Status::CANCELLED;
    }
}

// --------------------------------------------------------------------------------
// Private methods
