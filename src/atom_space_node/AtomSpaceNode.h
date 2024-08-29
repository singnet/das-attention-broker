#ifndef _ATOM_SPACE_NODE_ATOMSPACENODE_H
#define _ATOM_SPACE_NODE_ATOMSPACENODE_H

#include "LeadershipBroker.h"
#include "MessageBroker.h"
#include "Message.h"
#include <string>
#include <vector>

using namespace std;

namespace atom_space_node {

/**
 *
 */
class AtomSpaceNode : public MessageFactory {

public:

    ~AtomSpaceNode();

    static struct {
        string NODE_JOINED_NETWORK = "node_joined_network";
    } known_commands;

    void join_network();
    bool is_leader();
    string leader_id();
    bool has_leader();
    void add_peer(const string &peer_id);
    string node_id();

    virtual void node_joined_network(string &node_id) = 0;
    virtual string cast_leadership_vote() = 0;

protected:

    AtomSpaceNode(
        string &node_id,
        LeadershipBrokerType leadership_algorithm,
        MessageBrokerType messaging_backend
    );

    virtual Message *message_factory(string &command, vector<string> &args);

private:

    LeadershipBroker *leadership_broker;
    MessageBroker *message_broker;
    string my_node_id;
};

} // namespace atom_space_node

#endif // _ATOM_SPACE_NODE_ATOMSPACENODE_H
