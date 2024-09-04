#include "AtomSpaceNode.h"
#include "Utils.h"

using namespace atom_space_node;
using namespace commons;

AtomSpaceNode::AtomSpaceNode(
    const string &node_id,
    LeadershipBrokerType leadership_algorithm,
    MessageBrokerType messaging_backend) {

    this->my_node_id = node_id;
    this->leadership_broker = LeadershipBroker::factory(leadership_algorithm);
    this->message_broker = MessageBroker::factory(messaging_backend, this, node_id);
}

AtomSpaceNode::~AtomSpaceNode() {
    delete this->leadership_broker;
    delete this->message_broker;
}

// -------------------------------------------------------------------------------------------------
// Public API

void AtomSpaceNode::join_network() {
    this->message_broker->join_network();
    this->leadership_broker->set_message_broker(this->message_broker);
    string my_leadership_vote = this->cast_leadership_vote();
    this->leadership_broker->start_leader_election(my_leadership_vote);
    while (! this->has_leader()) {
        Utils::sleep();
    }
    vector<string> args;
    args.push_back(this->node_id());
    this->message_broker->broadcast(this->known_commands.NODE_JOINED_NETWORK, args);
}

bool AtomSpaceNode::is_leader() {
    return (this->leader_id() == this->node_id());
}

string AtomSpaceNode::leader_id() {
    return this->leadership_broker->leader_id();
}

bool AtomSpaceNode::has_leader() {
    return this->leadership_broker->has_leader();
}

void AtomSpaceNode::add_peer(const string &peer_id) {
    this->message_broker->add_peer(peer_id);
}

string AtomSpaceNode::node_id() {
    return this->my_node_id;
}

void AtomSpaceNode::broadcast(const string &command, const vector<string> &args) {
    this->message_broker->broadcast(command, args);
}

void AtomSpaceNode::send(
    const string &command, 
    const vector<string> &args, 
    const string &recipient) {

    this->message_broker->send(command, args, recipient);
}

// -------------------------------------------------------------------------------------------------
// Protected API

Message *AtomSpaceNode::message_factory(string &command, vector<string> &args) {
    if (command == this->known_commands.NODE_JOINED_NETWORK) {
        return new NodeJoinedNetwork(args[0]);
    } else {
        return NULL;
    }
}
