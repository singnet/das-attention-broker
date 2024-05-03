#include <iostream>
#include <string>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "gtest/gtest.h"

#include "common.pb.h"
#include "attention_broker.grpc.pb.h"
#include "attention_broker.pb.h"

#include "attention_broker_server.h"

TEST(AttentionBrokerTest, Basics) {
    
    AttentionBrokerServer service;
    das::Empty empty;
    das::Ack ack;
    ServerContext *context = NULL;

    service.ping(context, &empty, &ack);
    EXPECT_EQ(ack.msg(), "OK");
}

