#include "StarNode.h"
#include "LeadershipBroker.h"
#include "MessageBroker.h"

using namespace atom_space_node;
using namespace std;

// -------------------------------------------------------------------------------------------------
// Constructors and destructors

StarNode::StarNode(
    const string &node_id, 
    MessageBrokerType messaging_backend) :
    AtomSpaceNode(node_id, LeadershipBrokerType::SINGLE_MASTER_SERVER, messaging_backend) {

    cout << "XXXXXXXX COSNTRUCTOR StarNode server BEGIN " << std::to_string((unsigned long) this) << endl;
    this->is_server = true;
    this->join_network();
    cout << "XXXXXXXX COSNTRUCTOR StarNode server END " << std::to_string((unsigned long) this) << endl;
}

StarNode::StarNode(
    const string &node_id, 
    const string &server_id, 
    MessageBrokerType messaging_backend) : 
    AtomSpaceNode(node_id, LeadershipBrokerType::SINGLE_MASTER_SERVER, messaging_backend) {

    cout << "XXXXXXXX COSNTRUCTOR StarNode client BEGIN " << std::to_string((unsigned long) this) << endl;
    this->server_id = server_id;
    this->is_server = false;
    this->add_peer(server_id);
    this->join_network();
    cout << "XXXXXXXX COSNTRUCTOR StarNode client END " << std::to_string((unsigned long) this) << endl;
}

StarNode::~StarNode() {
}

// -------------------------------------------------------------------------------------------------
// DistributedAlgorithmNode virtual API

void StarNode::node_joined_network(const string &node_id) {
    if (this->is_server) {
        this->add_peer(node_id);
    }
}

string StarNode::cast_leadership_vote() {
    if (this->is_server) {
        return this->node_id();
    } else {
        return this->server_id;
    }
}
