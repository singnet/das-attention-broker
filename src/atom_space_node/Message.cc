#include "Message.h"
#include "AtomSpaceNode.h"

using namespace atom_space_node;

Message::Message() {
}

Message::~Message() {
}

// -------------------------------------------------------------------------------------------------
// Specialized Message subclasses

NodeJoinedNetwork::~NodeJoinedNetwork() {
}

NodeJoinedNetwork::NodeJoinedNetwork(string &node_id) {
    this->joining_node = node_id;
}

void NodeJoinedNetwork::act(AtomSpaceNode *node) {
    node->node_joined_network(this->joining_node);
}
