#include <cstdlib>

#include "gtest/gtest.h"
#include "test_utils.h"
#include "Utils.h"
#include "HebbianNetwork.h"

using namespace attention_broker_server;

TEST(HebbianNetwork, Basics) {
    
    HebbianNetwork network;
    string h1 = random_handle();
    string h2 = random_handle();
    string h3 = random_handle();
    string h4 = random_handle();
    string h5 = random_handle();

    network.add_node(h1);
    network.add_node(h2);
    network.add_node(h3);
    network.add_node(h4);

    EXPECT_TRUE(network.get_node_count(h1) == 1);
    EXPECT_TRUE(network.get_node_count(h2) == 1);
    EXPECT_TRUE(network.get_node_count(h3) == 1);
    EXPECT_TRUE(network.get_node_count(h4) == 1);
    EXPECT_TRUE(network.get_node_count(h5) == 0);
    network.add_node(h5);
    EXPECT_TRUE(network.get_node_count(h5) == 1);

    network.add_symmetric_edge(h1, h2);
    network.add_symmetric_edge(h1, h3);
    network.add_symmetric_edge(h1, h4);
    network.add_symmetric_edge(h2, h1);

    EXPECT_TRUE(network.get_symmetric_edge_count(h1, h2) == 2);
    EXPECT_TRUE(network.get_symmetric_edge_count(h2, h1) == 2);
    EXPECT_TRUE(network.get_symmetric_edge_count(h1, h3) == 1);
    EXPECT_TRUE(network.get_symmetric_edge_count(h3, h1) == 1);
    EXPECT_TRUE(network.get_symmetric_edge_count(h1, h4) == 1);
    EXPECT_TRUE(network.get_symmetric_edge_count(h4, h1) == 1);
    EXPECT_TRUE(network.get_symmetric_edge_count(h1, h5) == 0);
    EXPECT_TRUE(network.get_symmetric_edge_count(h5, h1) == 0);
}
