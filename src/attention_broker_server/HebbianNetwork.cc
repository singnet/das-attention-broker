#include <iostream>
#include "HebbianNetwork.h"
#include "expression_hasher.h"


using namespace attention_broker_server;

// --------------------------------------------------------------------------------
// Public methods

HebbianNetwork::HebbianNetwork() {
    nodes = new HandleTrie(HANDLE_HASH_SIZE - 1);
    edges = new HandleTrie(2 * (HANDLE_HASH_SIZE - 1));
    tokens_mutex.lock();
    tokens_to_distribute = 1.0;
    tokens_mutex.unlock();
}

HebbianNetwork::~HebbianNetwork() {
    delete nodes;
    delete edges;
}

void HebbianNetwork::add_node(string handle) {
    nodes->insert(handle, new HebbianNetwork::Node());
}

void HebbianNetwork::add_edge(string handle1, string handle2) {
    string composite = handle1 + handle2;
    edges->insert(composite, new HebbianNetwork::Edge());
}

unsigned int HebbianNetwork::get_node_count(string handle) {
    Node *node = (Node *) nodes->lookup(handle);
    if (node == NULL) {
        return 0;
    } else {
        return node->count;
    }
}

ImportanceType HebbianNetwork::get_node_importance(string handle) {
    Node *node = (Node *) nodes->lookup(handle);
    if (node == NULL) {
        return 0;
    } else {
        return node->importance;
    }
}

unsigned int HebbianNetwork::get_edge_count(string handle1, string handle2) {
    Edge *edge;
    string composite;
    if (handle1.compare(handle2) < 0) {
        composite = handle1 + handle2;
    } else {
        composite = handle2 + handle1;
    }
    edge = (Edge *) edges->lookup(composite);
    if (edge == NULL) {
        return 0;
    } else {
        return edge->count;
    }
}

ImportanceType HebbianNetwork::alienate_tokens() {
    ImportanceType answer;
    tokens_mutex.lock();
    answer = tokens_to_distribute;
    tokens_to_distribute = 0.0;
    tokens_mutex.unlock();
    return answer;
}

void HebbianNetwork::update_nodes(bool (*visit_function)(HandleTrie::TrieNode *node, void *data), void *data) {
    nodes->traverse(false, visit_function, data);
}
