#ifndef _ATOM_SPACE_NODE_MESSAGE_H
#define _ATOM_SPACE_NODE_MESSAGE_H

#include <string>
#include <vector>

using namespace std;

namespace atom_space_node {

class AtomSpaceNode;

/**
 *
 */
class Message {

public:

    string sender;
    string receiver;

    virtual void act(AtomSpaceNode *node) = 0;

protected:

    Message();
    ~Message();

private:

};

class MessageFactory {

public:

    virtual Message *message_factory(string &command, vector<string> &args) = 0;
};

class NodeJoinedNetwork : public Message {

private:

    string joining_node;

public:

    NodeJoinedNetwork(string &node_id);
    ~NodeJoinedNetwork();

    void act(AtomSpaceNode *node);
};

} // namespace atom_space_node

#endif // _ATOM_SPACE_NODE_MESSAGE_H
