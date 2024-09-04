#include "RequestSelector.h"
#include "AttentionBrokerServer.h"

using namespace attention_broker_server;

const string AttentionBrokerServer::GLOBAL_CONTEXT = "global";
const double AttentionBrokerServer::RENT_RATE = 0.1;
const double AttentionBrokerServer::SPREADING_RATE_LOWERBOUND = 0.01;
const double AttentionBrokerServer::SPREADING_RATE_UPPERBOUND = 0.10;

// --------------------------------------------------------------------------------
// Public methods
  
AttentionBrokerServer::AttentionBrokerServer() {
    stimulus_requests = new RequestQueue();
    correlation_requests = new RequestQueue();
    worker_threads = new WorkerThreads(stimulus_requests, correlation_requests);
    hebbian_network[GLOBAL_CONTEXT] = new HebbianNetwork();
}

AttentionBrokerServer::~AttentionBrokerServer() {
    graceful_shutdown();
    delete worker_threads;
    delete stimulus_requests;
    delete correlation_requests;
    for (auto pair:hebbian_network) {
        delete pair.second;
    }
}

void AttentionBrokerServer::graceful_shutdown() {
    rpc_api_enabled = false;
    worker_threads->graceful_stop();
}

// RPC API

Status AttentionBrokerServer::ping(ServerContext* grpc_context, const dasproto::Empty *request, dasproto::Ack* reply) {
    reply->set_msg("PING");
    if (rpc_api_enabled) {
        return Status::OK;
    } else{
        return Status::CANCELLED;
    }
}

Status AttentionBrokerServer::stimulate(ServerContext* grpc_context, const dasproto::HandleCount *request, dasproto::Ack* reply) {
    if (request->handle_count_size() > 0) {
        HebbianNetwork *network = select_hebbian_network(request->context());
        ((dasproto::HandleCount *) request)->set_hebbian_network((long) network);
        stimulus_requests->enqueue((void *) request);
    }
    reply->set_msg("STIMULATE");
    if (rpc_api_enabled) {
        return Status::OK;
    } else{
        return Status::CANCELLED;
    }
}

Status AttentionBrokerServer::correlate(ServerContext* grpc_context, const dasproto::HandleList *request, dasproto::Ack* reply) {
    if (request->handle_list_size() > 0) {
        HebbianNetwork *network = select_hebbian_network(request->context());
        ((dasproto::HandleList *) request)->set_hebbian_network((long) network);
        correlation_requests->enqueue((void *) request);
    }
    reply->set_msg("CORRELATE");
    if (rpc_api_enabled) {
        return Status::OK;
    } else {
        return Status::CANCELLED;
    }
}

// --------------------------------------------------------------------------------
// Private methods
//

HebbianNetwork *AttentionBrokerServer::select_hebbian_network(const string &context) {
    HebbianNetwork *network;
    if ((context != "") && (hebbian_network.find(context) != hebbian_network.end())) {
        network = hebbian_network[context];
    }
    if (context == "") {
        network = hebbian_network[GLOBAL_CONTEXT];
    } else {
        if (hebbian_network.find(context) == hebbian_network.end()) {
            network = new HebbianNetwork();
            hebbian_network[context] = network;
        } else {
            network = hebbian_network[context];
        }
    }
    return network;
}
