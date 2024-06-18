#ifndef _ATTENTION_BROKER_SERVER_ATTENTIONBROKERSERVER_H
#define _ATTENTION_BROKER_SERVER_ATTENTIONBROKERSERVER_H

#include "attention_broker.grpc.pb.h"
#include "RequestQueue.h"

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

        static const unsigned int REQUEST_QUEUES_INTIAL_SIZE = 1000;

        Status ping(ServerContext* context, const das::Empty* request, das::Ack* reply) override;
        Status stimulate(ServerContext* context, const das::HandleCount* request, das::Ack* reply) override;
        Status correlate(ServerContext* context, const das::HandleList* request, das::Ack* reply) override;

    private:

        RequestQueue *stimulus_requests;
        RequestQueue *correlation_requests;
};

}

#endif // _ATTENTION_BROKER_SERVER_ATTENTIONBROKERSERVER_H
