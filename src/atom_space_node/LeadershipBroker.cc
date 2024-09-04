#include "LeadershipBroker.h"
#include "Utils.h"

using namespace atom_space_node;

// -------------------------------------------------------------------------------------------------
// Constructors and destructors

LeadershipBroker::LeadershipBroker() {
    this->network_leader_id = "";
}

LeadershipBroker::~LeadershipBroker() {
}

LeadershipBroker *LeadershipBroker::factory(LeadershipBrokerType instance_type) {
    switch (instance_type) {
        case LeadershipBrokerType::SINGLE_MASTER_SERVER : {
            return new SingleMasterServer();
        }
        default: {
            Utils::error("Invalid LeadershipBrokerType: " + to_string((int) instance_type));
            return NULL; // to avoid warnings
        }
    }
}

SingleMasterServer::SingleMasterServer() {
}

SingleMasterServer::~SingleMasterServer() {
}
  
// -------------------------------------------------------------------------------------------------
// Public superclass API

string LeadershipBroker::leader_id() {
    return this->network_leader_id;
}

void LeadershipBroker::set_leader_id(const string &leader_id) {
    this->network_leader_id = leader_id;
}

bool LeadershipBroker::has_leader() {
    return (this->network_leader_id != "");
}

void LeadershipBroker::set_message_broker(MessageBroker *message_broker) {
    this->message_broker = message_broker;
}
  
// -------------------------------------------------------------------------------------------------
// Concrete implementation of abstract LeadershipBroker API

void SingleMasterServer::start_leader_election(const string &my_vote) {
    this->set_leader_id(my_vote);
}
