#ifndef _CACHE_NODE_CACHENODE_H
#define _CACHE_NODE_CACHENODE_H

#include <string>
#include "AtomSpaceNode.h"

using namespace std;
using namespace atom_space_node;

namespace cache_node {

// -------------------------------------------------------------------------------------------------
// Abstract superclass
  
/**
 * This is a node in the cache subsystem.
 *
 * This is an abstract class with two concrete implementations:
 * CacheNodeServer and CacheNodeClient. The cache network topology is defined
 * with a single server node which knows all the client nodes. Each client node
 * knows only the server node which is also, by definition,  the leader of the network.
 */
class CacheNode : public AtomSpaceNode {

public:

    /**
     * Destructor.
     */
    ~CacheNode();

    /**
     * Build the Message object which is supposed to execute the passed command.
     *
     * @param command The command to be remotely executed.
     * @param args Arguments for the command.
     * @return A Message object
     */
    virtual Message *message_factory(string &command, vector<string> &args);


    // Delegated to concrete subclasses

    virtual string cast_leadership_vote() = 0;
    virtual void node_joined_network(const string &node_id) = 0;

protected:

    CacheNode(const string &node_id, bool is_server);

private:

    bool is_server;
};

// -------------------------------------------------------------------------------------------------
// Concrete server
  
class CacheNodeServer : public CacheNode {

public:

    /**
     * Basic constructor.
     *
     * @param node_id The ID of this node. Typically it's a string to identify this node in the
     * MessageBroker.
     */
    CacheNodeServer(const string &node_id);

    /**
     * Destructor.
     */
    ~CacheNodeServer();

    /**
     * Casts a vote (which is the ID of the node being voted) in a leadership election.
     *
     * @return The ID of this node.
     */
    string cast_leadership_vote();

    /**
     * This method is called whenever a new node joins the network.
     *
     * Adds the passed node as a known peer.
     *
     * @param node_id The ID of the newly joined node.
     */
    virtual void node_joined_network(const string &node_id);
};

// -------------------------------------------------------------------------------------------------
// Concrete client
  
class CacheNodeClient : public CacheNode {

public:

    /**
     * Basic constructor.
     *
     * @param node_id The ID of this node. Typically it's a string to identify this node in the
     * MessageBroker.
     * @param server_id The ID of the server node.
     */
    CacheNodeClient(const string &node_id, string &server_id);

    /**
     * Destructor.
     */
    ~CacheNodeClient();

    /**
     * Casts a vote (which is the ID of the node being voted) in a leadership election.
     *
     * @return The ID of the server node passed in the constructor.
     */
    string cast_leadership_vote();

    /**
     * This method is called whenever a new node joins the network.
     *
     * Does nothing, disregarding any announcement of new nodes in the network.
     *
     * @param node_id The ID of the newly joined node.
     */
    virtual void node_joined_network(const string &node_id);

private:

    string server_id;
};

} // namespace cache_node

#endif // _CACHE_NODE_CACHENODE_H
