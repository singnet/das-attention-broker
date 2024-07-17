#include <cmath>
#include <cstdlib>
#include <set>
#include <list>
#include <forward_list>

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

    HebbianNetwork::Node *n1 = network.add_node(h1);
    HebbianNetwork::Node *n2 = network.add_node(h2);
    HebbianNetwork::Node *n3 = network.add_node(h3);
    HebbianNetwork::Node *n4 = network.add_node(h4);

    EXPECT_TRUE(network.get_node_count(h1) == 1);
    EXPECT_TRUE(network.get_node_count(h2) == 1);
    EXPECT_TRUE(network.get_node_count(h3) == 1);
    EXPECT_TRUE(network.get_node_count(h4) == 1);
    EXPECT_TRUE(network.get_node_count(h5) == 0);
    network.add_node(h5);
    EXPECT_TRUE(network.get_node_count(h5) == 1);

    network.add_edge(h1, h2, n1, n2);
    network.add_edge(h1, h3, n1, n3);
    network.add_edge(h1, h4, n1, n4);
    network.add_edge(h1, h2, n1, n2);

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
        HebbianNetwork::Node *n1 = network.add_node(h1);
        HebbianNetwork::Node *n2 = network.add_node(h2);
        network.add_edge(h1, h2, n1, n2);
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

bool visit1(HandleTrie::TrieNode *node, void *data) {
    ((HebbianNetwork::Node *) node->value)->importance = 1.0;
    return false;
}

bool visit2(
    HandleTrie::TrieNode *node,
    HebbianNetwork::Node *source, 
    forward_list<HebbianNetwork::Node *> &targets, 
    unsigned int targets_size,
    ImportanceType sum_weights,
    void *data) {

    unsigned int fan_max = *((unsigned int *) data);
    double stimulus = 1.0 / (double) fan_max;
    for (auto target: targets) {
        target->importance += stimulus;
        source->importance -= stimulus;
    }
    return false;
}

TEST(HebbianNetwork, update_neighbors) {

    unsigned int num_tests = 10;

    HebbianNetwork *network;
    map<string, set<string>> fan_in;
    map<string, set<string>> fan_out;

    for (unsigned int num_nodes: {10, 100, 1000}) {
        for (double fan_out_rate: {0.1, 0.5, 0.99}) {
            unsigned int fan_max = (unsigned int) lround(fan_out_rate * num_nodes);
            cout << num_nodes << " " << fan_out_rate << " " << fan_max << endl;
            for (unsigned i = 0; i < num_tests; i++) {
                fan_in.clear();
                fan_out.clear();
                network = new HebbianNetwork();
                string *handles = build_handle_space(num_nodes);
                for (unsigned int h = 0; h < num_nodes; h++) {
                    string source = handles[h];
                    unsigned int n = rand() % fan_max;
                    for (unsigned int j = 0; j < n; j++) {
                        string target;
                        do {
                            target = handles[rand() % num_nodes];
                        } while ((target == source) || (fan_out[source].find(target) != fan_out[source].end()));
                        fan_out[source].insert(target);
                        fan_in[target].insert(source);
                        HebbianNetwork::Node *n1 = network->add_node(source);
                        HebbianNetwork::Node *n2 = network->add_node(target);
                        network->add_edge(source, target, n1, n2);
                    }
                }
                network->update_nodes(false, &visit1, NULL);
                network->update_neighbors(false, true, &visit2, &fan_max);

                for (unsigned int h = 0; h < num_nodes; h++) {
                    ImportanceType v = network->get_node_importance(handles[h]);
                    int fi = fan_in[handles[h]].size();
                    int fo = fan_out[handles[h]].size();
                    double st = 1.0 / (double) fan_max;
                    double expected = ((fi + fo) == 0) ? 0.0 : 1.0 + ((fi - fo) * st);
                    /*
                    if (fabs(expected - v) > 0.000001) {
                        cout << expected << " " << v << " " << expected - v << " " << fi << " " << fo << endl;
                    }
                    */
                    EXPECT_TRUE(fabs(expected - v) < 0.000001);
                }
                delete network;
            }
        }
    }
}
