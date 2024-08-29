#ifndef _CACHE_NODE_CACHENODE_H
#define _CACHE_NODE_CACHENODE_H

#include <string>
#include "AtomSpaceNode.h"

using namespace std;
using namespace atom_space_node;

namespace cache_node {

/**
 *
 */
class CacheNode : public AtomSpaceNode {

public:

    ~CacheNode();
    virtual string cast_leadership_vote() = 0;
    virtual void node_joined_network(string &node_id) = 0;
    virtual Message *message_factory(string &command, vector<string> &args);

protected:

    CacheNode(string &node_id, bool is_server);

private:

    bool is_server;
};

class CacheNodeServer : public CacheNode {

public:

    CacheNodeServer(string &node_id);
    ~CacheNodeServer();

    string cast_leadership_vote();
    void node_joined_network(string &node_id);
};

class CacheNodeClient : public CacheNode {

public:

    CacheNodeClient(string &node_id, string &server_id);
    ~CacheNodeClient();

    string cast_leadership_vote();
    void node_joined_network(string &node_id);

private:

    string server_id;
};

} // namespace cache_node

#endif // _CACHE_NODE_CACHENODE_H
