#include "RequestSelector.h"
#include "AttentionBrokerServer.h"

using namespace attention_broker_server;

// --------------------------------------------------------------------------------
// Public methods
  
const string AttentionBrokerServer::GLOBAL_CONTEXT = "global";

AttentionBrokerServer::AttentionBrokerServer() {
    stimulus_requests = new RequestQueue(REQUEST_QUEUE_INTIAL_SIZE);
    correlation_requests = new RequestQueue(REQUEST_QUEUE_INTIAL_SIZE);
    worker_threads = new WorkerThreads(stimulus_requests, correlation_requests);
    hebbian_network[GLOBAL_CONTEXT] = new HebbianNetwork();
}

AttentionBrokerServer::~AttentionBrokerServer() {
    graceful_shutdown();
    delete stimulus_requests;
    delete correlation_requests;
    delete worker_threads;
    for (auto pair:hebbian_network) {
        delete pair.second;
    }
}

void AttentionBrokerServer::graceful_shutdown() {
    rpc_api_enabled = false;
    worker_threads->graceful_stop();
}

// RPC API

Status AttentionBrokerServer::ping(ServerContext* context, const das::Empty *request, das::Ack* reply) {
    reply->set_msg("PING");
    if (rpc_api_enabled) {
        return Status::OK;
    } else{
        return Status::CANCELLED;
    }
}

Status AttentionBrokerServer::stimulate(ServerContext* context, const das::HandleCount *request, das::Ack* reply) {
    ((das::HandleCount *) request)->set_hebbian_network((long) hebbian_network[GLOBAL_CONTEXT]);
    stimulus_requests->enqueue((void *) request);
    reply->set_msg("STIMULATE");
    if (rpc_api_enabled) {
        return Status::OK;
    } else{
        return Status::CANCELLED;
    }
}

Status AttentionBrokerServer::correlate(ServerContext* context, const das::HandleList *request, das::Ack* reply) {
    ((das::HandleList *) request)->set_hebbian_network((long) hebbian_network[GLOBAL_CONTEXT]);
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
