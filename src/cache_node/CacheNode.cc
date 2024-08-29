#include "CacheNode.h"

using namespace cache_node;

// -------------------------------------------------------------------------------------------------
// Public methods

CacheNode::CacheNode(string &node_id, bool is_server) : 
    AtomSpaceNode(node_id, LeadershipBrokerType::SINGLE_MASTER_SERVER, MessageBrokerType::GRPC) {

    this->is_server = is_server;
}

CacheNode::~CacheNode() {
}

CacheNodeServer::CacheNodeServer(string &node_id) : CacheNode(node_id, true) {
}

CacheNodeClient::CacheNodeClient(string &node_id, string &server_id) : CacheNode(node_id, false) {
    this->server_id = server_id;
    this->add_peer(server_id);
}

void CacheNode::node_joined_network(string &node_id) {
    if (this->is_server) {
        this->add_peer(node_id);
    }
}

string CacheNodeServer::cast_leadership_vote() {
    return this->node_id();
}

string CacheNodeClient::cast_leadership_vote() {
    return this->server_id;
}

Message *CacheNode::message_factory(string &command, vector<string> &args) {
    Message *message = AtomSpaceNode::message_factory(command, args);
    if (message != NULL) {
        return message;
    }
    return NULL;
}
