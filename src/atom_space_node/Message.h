#ifndef _ATOM_SPACE_NODE_MESSAGE_H
#define _ATOM_SPACE_NODE_MESSAGE_H

#include <string>
#include <vector>

using namespace std;

namespace atom_space_node {

class AtomSpaceNode;

// -------------------------------------------------------------------------------------------------
// Abstract superclass

/**
 * Basic abstract class for Messages to be exchanged among nodes in the network.
 *
 * Concrete subclasses should implement the act() method to perform whatever operation is
 * required in the recipient node.
 *
 * Note that Message objects don't actually get serialized and sent through the network.
 * What's actually sent is some identifier that makes possible for the recipients to
 * know which concrete class of Message should be instantiated there.
 *
 * Once the recipient receives this identifier, it instantiates an object of the proper
 * class (a concrete implementation of Message) and executed act(), passing the target
 * node as parameter.
 */
class Message {

public:

    /**
     * Executes the action defined in the Message in the recipient node, which is passed as
     * parameter.
     *
     * @param node The AtomSpaceNode which received the Message.
     */
    virtual void act(AtomSpaceNode *node) = 0;

protected:

    /**
     * Empty constructor.
     */
    Message();

    /**
     * Destructor
     */
    ~Message();

private:

};

/**
 * Interface to be implemented by nodes (concrete implementations of AtomSpaceNode) in order to
 * provide a factory method for the types of messages defined in its specific network.
 */
class MessageFactory {

public:

    /**
     * Message factory method.
     *
     * @param command The command to be executed in the target nodes.
     * @param args Arguments for the command.
     * @return An object of the proper class to deal with the passed command.
     */
    virtual Message *message_factory(string &command, vector<string> &args) = 0;
};

// -------------------------------------------------------------------------------------------------
// Concrete Messages used in basic AtomSpaceNode settings

/**
 * Concrete Message implementation to deal with command "node_join_network", used
 * by AtomSpaceNode to notify other nodes in the network of the presence of a newly
 * joined node.
 */
class NodeJoinedNetwork : public Message {

private:

    string joining_node;

public:

    /**
     * Basic constructor.
     *
     * @param node_id ID of the newly joined node.
     */
    NodeJoinedNetwork(string &node_id);

    /**
     * Destructor.
     */
    ~NodeJoinedNetwork();

    /**
     * Calls node->node_joined_neytwork() in the recipient node.
     */
    void act(AtomSpaceNode *node);
};

} // namespace atom_space_node

#endif // _ATOM_SPACE_NODE_MESSAGE_H
