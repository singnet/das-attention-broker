#include "RequestSelector.h"
#include "AttentionBrokerServer.h"

using namespace attention_broker_server;

const double AttentionBrokerServer::RENT_RATE;
const double AttentionBrokerServer::SPREADING_RATE_LOWERBOUND;
const double AttentionBrokerServer::SPREADING_RATE_UPPERBOUND;

// --------------------------------------------------------------------------------
// Public methods
  
AttentionBrokerServer::AttentionBrokerServer() {
    cout << "XXXXXXXXXXXXXXXXXXXXXXXX AttentionBrokerServer::AttentionBrokerServer() BEGIN" << endl;
    cout << "XXXXXXXXXXXXXXXXXXXXXXXX AttentionBrokerServer() this = " << to_string((long) this) << endl;
    this->global_context = "global";
    stimulus_requests = new RequestQueue();
    correlation_requests = new RequestQueue();
    worker_threads = new WorkerThreads(stimulus_requests, correlation_requests);
    HebbianNetwork *network = new HebbianNetwork();
    hebbian_network[this->global_context] = network;
    cout << "XXXXXXXXXXXXXXXXXXXXXXXX AttentionBrokerServer() global context = " << this->global_context << endl;
    cout << "XXXXXXXXXXXXXXXXXXXXXXXX AttentionBrokerServer() network = " << to_string((long) hebbian_network[this->global_context]) << endl;
    cout << "XXXXXXXXXXXXXXXXXXXXXXXX AttentionBrokerServer::AttentionBrokerServer() END" << endl;
}

AttentionBrokerServer::~AttentionBrokerServer() {
    cout << "XXXXXXXXXXXXXXXXXXXXXXXX DESTRUCTOR AttentionBrokerServer::~AttentionBrokerServer() BEGIN" << endl;
    graceful_shutdown();
    delete worker_threads;
    delete stimulus_requests;
    delete correlation_requests;
    for (auto pair:hebbian_network) {
        delete pair.second;
    }
    cout << "XXXXXXXXXXXXXXXXXXXXXXXX DESTRUCTOR AttentionBrokerServer::~AttentionBrokerServer() END" << endl;
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
    if (request->map_size() > 0) {
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
    if (request->list_size() > 0) {
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

Status AttentionBrokerServer::get_importance(ServerContext *grpc_context, const dasproto::HandleList *request, dasproto::ImportanceList *reply) {
    cout << "XXXXXXXXXXXXXXXXXXXXXXXX AttentionBrokerServer::get_importance() BEGIN" << endl;
    cout << "XXXXXXXXXXXXXXXXXXXXXXXX AttentionBrokerServer::get_importance() this = " << to_string((long) this) << endl;
    cout << "XXXXXXXXXXXXXXXXXXXXXXXX AttentionBrokerServer::get_importance() global context = " << this->global_context << endl;
    cout << "XXXXXXXXXXXXXXXXXXXXXXXX AttentionBrokerServer::get_importance() network = " << to_string((long) hebbian_network[this->global_context]) << endl;
    if (rpc_api_enabled) {
        cout << "XXXXXXXXXXXXXXXXXXXXXXXX request->list_size() " << request->list_size() << endl;
        int num_handles = request->list_size();
        if (num_handles > 0) {
            HebbianNetwork *network = select_hebbian_network(request->context());
            if (network == NULL) { // XXX
                cout << "network is NULL" << endl;
            }
            for (int i = 0; i < num_handles; i++) {
                cout << "XXXXXXXXXXXXXXXXXXXXXXXX request->list(i) " << request->list(i) << endl;
                float importance = network->get_node_importance(request->list(i));
                cout << "XXXXXXXXXXXXXXXXXXXXXXXX A" << endl;
                reply->add_list(importance);
                cout << "XXXXXXXXXXXXXXXXXXXXXXXX B" << endl;
            }
        }
        cout << "XXXXXXXXXXXXXXXXXXXXXXXX OK " << endl;
        return Status::OK;
    } else {
        cout << "XXXXXXXXXXXXXXXXXXXXXXXX CANCELLED " << endl;
        return Status::CANCELLED;
    }
    cout << "XXXXXXXXXXXXXXXXXXXXXXXX AttentionBrokerServer::get_importance() END" << endl;
}

// --------------------------------------------------------------------------------
// Private methods
//

HebbianNetwork *AttentionBrokerServer::select_hebbian_network(const string &context) {
    HebbianNetwork *network;
    cout << "XXXXXXXXXXXXXXX select_hebbian_network() context:" << context << endl;
    cout << "XXXXXXXXXXXXXXXXXXXXXXXX select_hebbian_network() this = " << to_string((long) this) << endl;
    if ((context != "") && (hebbian_network.find(context) != hebbian_network.end())) {
        cout << "XXXXXXXXXXXXXXX select_hebbian_network() 1" << endl;
        network = hebbian_network[context];
    }
    if (context == "") {
        cout << "XXXXXXXXXXXXXXX select_hebbian_network() 2" << endl;
        network = hebbian_network[this->global_context];
        if (network == NULL) { // XXX
            cout << "network 1 is NULL" << endl;
        }
        if (hebbian_network[this->global_context] == NULL) { // XXX
            cout << "hebbian_network[this->global_context] 2 is NULL" << endl;
        }
    } else {
        cout << "XXXXXXXXXXXXXXX select_hebbian_network() 3" << endl;
        if (hebbian_network.find(context) == hebbian_network.end()) {
            cout << "XXXXXXXXXXXXXXX select_hebbian_network() 4" << endl;
            network = new HebbianNetwork();
            hebbian_network[context] = network;
        } else {
            cout << "XXXXXXXXXXXXXXX select_hebbian_network() 5" << endl;
            network = hebbian_network[context];
        }
    }
    cout << "XXXXXXXXXXXXXXX select_hebbian_network() 6" << endl;
    if (network == NULL) { // XXX
        cout << "network 2 is NULL" << endl;
    }
    return network;
}
