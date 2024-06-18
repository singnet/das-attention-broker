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

class TestRequestQueue: public RequestQueue {
    public:
        TestRequestQueue(unsigned int n) : RequestQueue(n) {
        }
        void *test_dequeue() {
            return dequeue();
        }
        unsigned int test_current_size() {
            return current_size();
        }
        unsigned int test_current_start() {
            return current_start();
        }
        unsigned int test_current_end() {
            return current_end();
        }
};

class TestMessage {
    public:
        int message;
        TestMessage(int n) {
            message = n;
        }
};

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

TEST(AttentionBrokerTest, RequestQueues) {
    
    AttentionBrokerServer service;
    das::Empty empty;
    das::HandleCount handle_count;
    das::HandleList handle_list;
    das::Ack ack;
    ServerContext *context = NULL;

    TestRequestQueue q1((unsigned int) 5);
    EXPECT_TRUE(q1.test_current_size() == 5);
    q1.enqueue((void *) "1");
    EXPECT_EQ((char *) q1.test_dequeue(), "1");
    q1.enqueue((void *) "2");
    q1.enqueue((void *) "3");
    q1.enqueue((void *) "4");
    q1.enqueue((void *) "5");
    EXPECT_TRUE(q1.test_current_size() == 5);
    EXPECT_EQ((char *) q1.test_dequeue(), "2");
    q1.enqueue((void *) "6");
    q1.enqueue((void *) "7");
    EXPECT_TRUE(q1.test_current_size() == 5);
    EXPECT_EQ((char *) q1.test_dequeue(), "3");
    q1.enqueue((void *) "8");
    EXPECT_EQ((char *) q1.test_dequeue(), "4");
    q1.enqueue((void *) "9");
    EXPECT_EQ((char *) q1.test_dequeue(), "5");
    q1.enqueue((void *) "10");
    EXPECT_TRUE(q1.test_current_size() == 5);
    EXPECT_EQ((char *) q1.test_dequeue(), "6");
    EXPECT_EQ((char *) q1.test_dequeue(), "7");
    q1.enqueue((void *) "11");
    q1.enqueue((void *) "12");
    EXPECT_TRUE(q1.test_current_size() == 5);
    EXPECT_TRUE(q1.test_current_start() == 2);
    EXPECT_TRUE(q1.test_current_end() == 2);
    q1.enqueue((void *) "13");
    EXPECT_TRUE(q1.test_current_size() == 10);
    EXPECT_TRUE(q1.test_current_start() == 0);
    EXPECT_TRUE(q1.test_current_end() == 6);
    q1.enqueue((void *) "14");
    EXPECT_EQ((char *) q1.test_dequeue(), "8");
    EXPECT_EQ((char *) q1.test_dequeue(), "9");
    EXPECT_EQ((char *) q1.test_dequeue(), "10");
    EXPECT_EQ((char *) q1.test_dequeue(), "11");
    EXPECT_EQ((char *) q1.test_dequeue(), "12");
    EXPECT_EQ((char *) q1.test_dequeue(), "13");
    EXPECT_EQ((char *) q1.test_dequeue(), "14");
}

