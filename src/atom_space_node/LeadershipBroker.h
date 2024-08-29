#ifndef _ATOM_SPACE_NODE_LEADERSHIPBROKER_H
#define _ATOM_SPACE_NODE_LEADERSHIPBROKER_H

#include "MessageBroker.h"

using namespace std;

namespace atom_space_node {

enum class LeadershipBrokerType {
    SINGLE_MASTER_SERVER
};

/**
 *
 */
class LeadershipBroker {

public:

    static LeadershipBroker *factory(LeadershipBrokerType instance_type);
    virtual ~LeadershipBroker();
    void set_message_broker(MessageBroker *message_broker);
    string leader_id();
    void set_leader_id(string &leader_id);
    bool has_leader();

    virtual void start_leader_election(string &my_vote) = 0;

protected:

    LeadershipBroker();

private:

    MessageBroker *message_broker;
    string network_leader_id;

};

class SingleMasterServer : public LeadershipBroker {

public:

    SingleMasterServer();
    ~SingleMasterServer();

    void start_leader_election(string &my_vote);
};


} // namespace atom_space_node

#endif // _ATOM_SPACE_NODE_LEADERSHIPBROKER_H

