#include "gtest/gtest.h"
#include "CacheNode.h"

using namespace atom_space_node;
using namespace cache_node;
using namespace std;

TEST(CacheNode, basics) {

    string server_id = "localhost:35700";
    string client1_id = "localhost:35701";
    string client2_id = "localhost:35702";
    CacheNodeServer server(server_id);
    CacheNodeClient client1(client1_id, server_id);
    CacheNodeClient client2(client2_id, server_id);

    // Check properties before joining the network

    EXPECT_FALSE(server.is_leader());
    EXPECT_FALSE(client1.is_leader());
    EXPECT_FALSE(client2.is_leader());

    EXPECT_FALSE(server.has_leader());
    EXPECT_FALSE(client1.has_leader());
    EXPECT_FALSE(client2.has_leader());

    EXPECT_TRUE(server.leader_id() == "");
    EXPECT_TRUE(client1.leader_id() == "");
    EXPECT_TRUE(client2.leader_id() == "");

    EXPECT_TRUE(server.node_id() == server_id);
    EXPECT_TRUE(client1.node_id() == client1_id);
    EXPECT_TRUE(client2.node_id() == client2_id);

    // Create network

    server.join_network();
    client1.join_network();
    client2.join_network();

    // Check properties after joining the network

    EXPECT_TRUE(server.is_leader());
    EXPECT_FALSE(client1.is_leader());
    EXPECT_FALSE(client2.is_leader());

    EXPECT_TRUE(server.has_leader());
    EXPECT_TRUE(client1.has_leader());
    EXPECT_TRUE(client2.has_leader());

    EXPECT_TRUE(server.leader_id() == server_id);
    EXPECT_TRUE(client1.leader_id() == server_id);
    EXPECT_TRUE(client2.leader_id() == server_id);

    EXPECT_TRUE(server.node_id() == server_id);
    EXPECT_TRUE(client1.node_id() == client1_id);
    EXPECT_TRUE(client2.node_id() == client2_id);
}
