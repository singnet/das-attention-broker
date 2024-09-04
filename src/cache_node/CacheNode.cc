#include "CacheNode.h"

using namespace cache_node;

// -------------------------------------------------------------------------------------------------
// Constructors and destructors

CacheNode::CacheNode(const string &node_id, bool is_server) : 
    AtomSpaceNode(node_id, LeadershipBrokerType::SINGLE_MASTER_SERVER, MessageBrokerType::GRPC) {

    this->is_server = is_server;
}

CacheNodeServer::CacheNodeServer(const string &node_id) : CacheNode(node_id, true) {
}

CacheNodeClient::CacheNodeClient(
    const string &node_id, 
    string &server_id) : CacheNode(node_id, false) {

    this->server_id = server_id;
    this->add_peer(server_id);
}

CacheNode::~CacheNode() {
}

CacheNodeServer::~CacheNodeServer() {
}

CacheNodeClient::~CacheNodeClient() {
}

// -------------------------------------------------------------------------------------------------
// Public API delegated to concrete nodes

void CacheNodeServer::node_joined_network(const string &node_id) {
    this->add_peer(node_id);
}

void CacheNodeClient::node_joined_network(const string &node_id) {
    // Do nothing
}

string CacheNodeServer::cast_leadership_vote() {
    return this->node_id();
}

string CacheNodeClient::cast_leadership_vote() {
    return this->server_id;
}

// -------------------------------------------------------------------------------------------------
// Other methods

Message *CacheNode::message_factory(string &command, vector<string> &args) {
    Message *message = AtomSpaceNode::message_factory(command, args);
    if (message != NULL) {
        return message;
    }
    return NULL;
}
