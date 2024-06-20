#ifndef _ATTENTION_BROKER_SERVER_ATTENTIONBROKERSERVER_H
#define _ATTENTION_BROKER_SERVER_ATTENTIONBROKERSERVER_H

#include "attention_broker.grpc.pb.h"
#include "RequestQueue.h"
#include "WorkerThreadPool.h"

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

        // RPC API
          
        Status ping(ServerContext* context, const das::Empty* request, das::Ack* reply) override;
        Status stimulate(ServerContext* context, const das::HandleCount* request, das::Ack* reply) override;
        Status correlate(ServerContext* context, const das::HandleList* request, das::Ack* reply) override;

        // Other public methods

        void graceful_shutdown();

    private:

        bool rpc_api_enabled = true;
        RequestQueue *stimulus_requests;
        RequestQueue *correlation_requests;
        WorkerThreadPool *worker_threads;
};

} // namespace attention_broker_server

#endif // _ATTENTION_BROKER_SERVER_ATTENTIONBROKERSERVER_H
