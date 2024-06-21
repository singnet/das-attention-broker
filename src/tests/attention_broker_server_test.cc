#include <iostream>
#include <string>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "gtest/gtest.h"

#include "common.pb.h"
#include "attention_broker.grpc.pb.h"
#include "attention_broker.pb.h"

#include "AttentionBrokerServer.h"

using namespace attention_broker_server;

TEST(AttentionBrokerTest, Basics) {
    
    AttentionBrokerServer service;
    das::Empty empty;
    das::HandleCount handle_count;
    das::HandleList handle_list;
    das::Ack ack;
    ServerContext *context = NULL;

    service.ping(context, &empty, &ack);
    EXPECT_EQ(ack.msg(), "PING");
    service.stimulate(context, &handle_count, &ack);
    EXPECT_EQ(ack.msg(), "STIMULATE");
    service.correlate(context, &handle_list, &ack);
    EXPECT_EQ(ack.msg(), "CORRELATE");
}
