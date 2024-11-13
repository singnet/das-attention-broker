#ifndef _QUERY_NODE_STARNODE_H
#define _QUERY_NODE_STARNODE_H

#include <string>
#include "AtomSpaceNode.h"

using namespace std;
using namespace atom_space_node;

namespace star_node {

/**
 *
 */
class StarNode : public AtomSpaceNode {

public:

    StarNode(
        const string &node_id, 
        MessageBrokerType messaging_backend = MessageBrokerType::GRPC);

    StarNode(
        const string &node_id, 
        const string &server_id, 
        MessageBrokerType messaging_backend = MessageBrokerType::GRPC);

    virtual ~StarNode();

    void node_joined_network(const string &node_id);
    string cast_leadership_vote();

private:

    bool is_server;
    string server_id;
};

} // namespace star_node

#endif // _QUERY_NODE_STARNODE_H
