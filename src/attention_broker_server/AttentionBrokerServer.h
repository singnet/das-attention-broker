#ifndef _ATTENTION_BROKER_SERVER_ATTENTIONBROKERSERVER_H
#define _ATTENTION_BROKER_SERVER_ATTENTIONBROKERSERVER_H

#include <string>
#include <unordered_map>
#include "attention_broker.grpc.pb.h"
#include "RequestQueue.h"
#include "WorkerThreads.h"
#include "HebbianNetwork.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using das::AttentionBroker;

namespace attention_broker_server {

class AttentionBrokerServer final: public AttentionBroker::Service {

    public:

        AttentionBrokerServer();
        ~AttentionBrokerServer();

        static const unsigned int REQUEST_QUEUE_INTIAL_SIZE = 1000;
        static const unsigned int WORKER_THREADS_COUNT = 10;
        static const string GLOBAL_CONTEXT;

        // RPC API
          
        Status ping(ServerContext* grpc_context, const das::Empty* request, das::Ack* reply) override;
        Status stimulate(ServerContext* grpc_context, const das::HandleCount* request, das::Ack* reply) override;
        Status correlate(ServerContext* grpc_context, const das::HandleList* request, das::Ack* reply) override;

        // Other public methods

        void graceful_shutdown();

    private:

        bool rpc_api_enabled = true;
        RequestQueue *stimulus_requests;
        RequestQueue *correlation_requests;
        WorkerThreads *worker_threads;
        unordered_map<string, HebbianNetwork *> hebbian_network;

        HebbianNetwork *select_hebbian_network(const string &context);
};

} // namespace attention_broker_server

#endif // _ATTENTION_BROKER_SERVER_ATTENTIONBROKERSERVER_H
