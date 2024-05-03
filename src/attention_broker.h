#include "attention_broker.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using das::AttentionBroker;

class AttentionBrokerServer final: public AttentionBroker::Service {
    Status ping(ServerContext* context, const das::Empty* request, das::Ack* reply) override;
};
