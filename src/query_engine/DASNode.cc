#include "DASNode.h"

using namespace query_engine;

// -------------------------------------------------------------------------------------------------
// Public methods

DASNode::DASNode(const string &node_id) : StarNode(node_id) {
    // SERVER
}

DASNode::DASNode(const string &node_id, const string &server_id) : StarNode(node_id, server_id) {
    // CLIENT
}

DASNode::~DASNode() {
}
