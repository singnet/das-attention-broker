#include "HebbianNetwork.h"
#include "expression_hasher.h"


using namespace attention_broker_server;

// --------------------------------------------------------------------------------
// Public methods

HebbianNetwork::HebbianNetwork() {
    node_count = new HandleTrie(HANDLE_HASH_SIZE - 1);
    symmetric_edge_count = new HandleTrie(2 * (HANDLE_HASH_SIZE - 1));
}

HebbianNetwork::~HebbianNetwork() {
    delete node_count;
    delete symmetric_edge_count;
}

void HebbianNetwork::add_node(string handle) {
    node_count->insert(handle, new HebbianNetwork::AccumulatorValue());
}

void HebbianNetwork::add_symmetric_edge(string handle1, string handle2) {
    string composite = handle1 + handle2;
    symmetric_edge_count->insert(composite, new HebbianNetwork::AccumulatorValue());
}

unsigned int HebbianNetwork::get_node_count(string handle) {
    AccumulatorValue *value = (AccumulatorValue *) node_count->lookup(handle);
    if (value == NULL) {
        return 0;
    } else {
        return value->count;
    }
}

unsigned int HebbianNetwork::get_symmetric_edge_count(string handle1, string handle2) {
    AccumulatorValue *value;
    string composite;
    if (handle1.compare(handle2) < 0) {
        composite = handle1 + handle2;
    } else {
        composite = handle2 + handle1;
    }
    value = (AccumulatorValue *) symmetric_edge_count->lookup(composite);
    if (value == NULL) {
        return 0;
    } else {
        return value->count;
    }
}
