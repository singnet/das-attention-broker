#include "attention_broker.h"

Status AttentionBrokerServer::ping(ServerContext* context, const das::Empty* request, das::Ack* reply) {
    reply->set_msg("Hey");
    return Status::OK;
}
