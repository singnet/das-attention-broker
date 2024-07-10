#include <cstdlib>
#include <set>

#include "gtest/gtest.h"
#include "Utils.h"
#include "test_utils.h"
#include "HebbianNetwork.h"
#include "expression_hasher.h"

using namespace attention_broker_server;

TEST(HebbianNetwork, basics) {
    
    HebbianNetwork network;

    string h1 = prefixed_random_handle("a");
    string h2 = prefixed_random_handle("b");
    string h3 = prefixed_random_handle("d");
    string h4 = prefixed_random_handle("d");
    string h5 = prefixed_random_handle("e");

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

    network.add_edge(h1, h2);
    network.add_edge(h1, h3);
    network.add_edge(h1, h4);
    network.add_edge(h1, h2);

    EXPECT_TRUE(network.get_edge_count(h1, h2) == 2);
    EXPECT_TRUE(network.get_edge_count(h2, h1) == 2);
    EXPECT_TRUE(network.get_edge_count(h1, h3) == 1);
    EXPECT_TRUE(network.get_edge_count(h3, h1) == 1);
    EXPECT_TRUE(network.get_edge_count(h1, h4) == 1);
    EXPECT_TRUE(network.get_edge_count(h4, h1) == 1);
    EXPECT_TRUE(network.get_edge_count(h1, h5) == 0);
    EXPECT_TRUE(network.get_edge_count(h5, h1) == 0);
}

TEST(HebbianNetwork, stress) {
    
    HebbianNetwork network;
    StopWatch timer_insertion;
    StopWatch timer_lookup;
    StopWatch timer_total;
    unsigned int handle_space_size = 500;
    unsigned int num_insertions = (handle_space_size * 2) * (handle_space_size * 2);
    unsigned int num_lookups = 10 * num_insertions;

    string *handles = build_handle_space(handle_space_size);

    timer_insertion.start();
    timer_total.start();

    for (unsigned int i = 0; i < num_insertions; i++) {
        string h1 = handles[rand() % handle_space_size];
        string h2 = handles[rand() % handle_space_size];
        network.add_node(h1);
        network.add_node(h2);
        network.add_edge(h1, h2);
    }

    timer_insertion.stop();
    timer_lookup.start();

    for (unsigned int i = 0; i < num_lookups; i++) {
        string h1 = handles[rand() % handle_space_size];
        string h2 = handles[rand() % handle_space_size];
        network.get_node_count(h1);
        network.get_node_count(h2);
        network.get_edge_count(h1, h2);
    }

    timer_lookup.stop();
    timer_total.stop();

    cout << "==================================================================" << endl;
    cout << "Insertions: " << timer_insertion.str_time() << endl;
    cout << "Lookups: " << timer_lookup.str_time() << endl;
    cout << "Total: " << timer_total.str_time() << endl;
    cout << "==================================================================" << endl;
    //EXPECT_TRUE(false);
}

TEST(HebbianNetwork, alienate_tokens) {
    HebbianNetwork network;
    EXPECT_TRUE(network.alienate_tokens() == 1.0);
    EXPECT_TRUE(network.alienate_tokens() == 0.0);
    EXPECT_TRUE(network.alienate_tokens() == 0.0);
}
