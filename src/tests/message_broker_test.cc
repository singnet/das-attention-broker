#include <cstdlib>
#include <cmath>

#include "gtest/gtest.h"
#include "test_utils.h"
#include "Utils.h"
#include "MessageBroker.h"
#include "CacheNode.h"

using namespace atom_space_node;
using namespace cache_node;

class TestNode {
public: 
    string command;
    vector<string> args;
    unsigned int node_joined_network_count;
};

class TestCacheNodeClient;

class TestMessage : public Message {
public:
    string command;
    vector<string> args;
    TestMessage(string command, vector<string> args) {
        this->command = command;
        this->args = args;
    }
    void act(AtomSpaceNode *_node);
};

class TestCacheNodeServer : public TestNode, public CacheNodeServer {
public:
    TestCacheNodeServer(const string &node_id) : CacheNodeServer(node_id) {
        this->command = "";
        this->node_joined_network_count = 0;
    }
    void node_joined_network(const string &node_id) {
        CacheNodeServer::node_joined_network(node_id);
        this->node_joined_network_count += 1;
    }
    Message *message_factory(string &command, vector<string> &args) {
        Message *message = CacheNode::message_factory(command, args);
        if (message != NULL) {
            return message;
        }
        if (command == "c1" || command == "c2" || command == "c3") {
            return new TestMessage(command, args);
        }
        return NULL;
    }
};

class TestCacheNodeClient : public TestNode, public CacheNodeClient {
public:
    TestCacheNodeClient(const string &node_id, string &server_id) : CacheNodeClient(node_id, server_id) {
        this->command = "";
        this->node_joined_network_count = 0;
    }
    void node_joined_network(const string &node_id) {
        CacheNodeClient::node_joined_network(node_id);
        this->node_joined_network_count += 1;
    }
    Message *message_factory(string &command, vector<string> &args) {
        Message *message = CacheNode::message_factory(command, args);
        if (message != NULL) {
            return message;
        }
        if (command == "c1" || command == "c2" || command == "c3") {
            return new TestMessage(command, args);
        }
        return NULL;
    }
};

void TestMessage::act(AtomSpaceNode *_node) {
    TestCacheNodeClient *node = (TestCacheNodeClient *) _node;
    ((TestNode *) node)->command = this->command;
    ((TestNode *) node)->args = this->args;
}

TEST(MessageBroker, basics) {

    try {
        MessageBroker::factory((MessageBrokerType) -1, NULL, "");
        FAIL() << "Expected exception";
    } catch(std::runtime_error const &error) {
    } catch(...) {
        FAIL() << "Expected std::runtime_error";
    }

    SynchronousGRPC *message_broker = (SynchronousGRPC *)
        MessageBroker::factory(MessageBrokerType::GRPC, NULL, "");
    EXPECT_TRUE(message_broker != NULL);
}

TEST(CacheNode, basics) {

    string server_id = "localhost:30700";
    string client1_id = "localhost:30701";
    string client2_id = "localhost:30702";

    CacheNodeServer server(server_id);
    EXPECT_FALSE(server.is_leader());
    EXPECT_FALSE(server.has_leader());
    EXPECT_EQ(server.leader_id(), "");
    EXPECT_EQ(server.node_id(), server_id);
    server.join_network();
    EXPECT_TRUE(server.is_leader());
    EXPECT_TRUE(server.has_leader());
    EXPECT_EQ(server.leader_id(), server_id);
    EXPECT_EQ(server.node_id(), server_id);

    CacheNodeClient client1(client1_id, server_id);
    CacheNodeClient client2(client2_id, server_id);

    EXPECT_FALSE(client1.is_leader());
    EXPECT_FALSE(client2.is_leader());

    EXPECT_FALSE(client1.has_leader());
    EXPECT_FALSE(client2.has_leader());

    EXPECT_EQ(client1.leader_id(), "");
    EXPECT_EQ(client2.leader_id(), "");

    EXPECT_EQ(client1.node_id(), client1_id);
    EXPECT_EQ(client2.node_id(), client2_id);

    client1.join_network();
    client2.join_network();

    EXPECT_FALSE(client1.is_leader());
    EXPECT_FALSE(client2.is_leader());

    EXPECT_TRUE(client1.has_leader());
    EXPECT_TRUE(client2.has_leader());

    EXPECT_EQ(client1.leader_id(), server_id);
    EXPECT_EQ(client2.leader_id(), server_id);

    EXPECT_EQ(client1.node_id(), client1_id);
    EXPECT_EQ(client2.node_id(), client2_id);
}

TEST(MessageBroker, communication) {
    string server_id = "localhost:31700";
    string client1_id = "localhost:31701";
    string client2_id = "localhost:31702";

    TestCacheNodeServer server(server_id);
    server.join_network();
    Utils::sleep(1000);

    TestCacheNodeClient client1(client1_id, server_id);
    client1.join_network();
    Utils::sleep(1000);

    TestCacheNodeClient client2(client2_id, server_id);
    client2.join_network();
    Utils::sleep(1000);

    EXPECT_EQ(server.command, "");
    EXPECT_EQ(server.args.size(), 0);
    EXPECT_EQ(client1.command, "");
    EXPECT_EQ(client1.args.size(), 0);
    EXPECT_EQ(client2.command, "");
    EXPECT_EQ(client2.args.size(), 0);

    EXPECT_EQ(server.node_joined_network_count, 2);
    EXPECT_EQ(client1.node_joined_network_count, 1);
    EXPECT_EQ(client2.node_joined_network_count, 0); // TODO Fix this. This count should be 2

    vector<string> args1 = {"a", "b"};
    server.broadcast("c1", args1);
    Utils::sleep(1000);

    EXPECT_EQ(server.command, "");
    EXPECT_EQ(server.args.size(), 0);
    EXPECT_EQ(client1.command, "c1");
    EXPECT_EQ(client1.args, args1);
    EXPECT_EQ(client2.command, "c1");
    EXPECT_EQ(client2.args, args1);

    vector<string> args2 = {"a2", "b2"};
    server.send("c2", args2, client1_id);
    Utils::sleep(1000);

    EXPECT_EQ(server.command, "");
    EXPECT_EQ(server.args.size(), 0);
    EXPECT_EQ(client1.command, "c2");
    EXPECT_EQ(client1.args, args2);
    EXPECT_EQ(client2.command, "c1");
    EXPECT_EQ(client2.args, args1);
}

