#ifndef _ATTENTION_BROKER_SERVER_HEBBIANNETWORK_H
#define _ATTENTION_BROKER_SERVER_HEBBIANNETWORK_H

#include <string>
#include <mutex>
#include "HandleTrie.h"

using namespace std;

namespace attention_broker_server {

//typedef DiscreteFloat ImportanceType;
typedef double ImportanceType;

/**
 *
 */
class HebbianNetwork {

public:

    HebbianNetwork();
    ~HebbianNetwork();

    void add_node(string handle);
    void add_edge(string handle1, string handle2);
    unsigned int get_node_count(string handle);
    ImportanceType get_node_importance(string handle);
    unsigned int get_edge_count(string handle1, string handle2);
    ImportanceType alienate_tokens();
    void update_nodes(bool (*visit_function)(HandleTrie::TrieNode *node, void *data), void *data);

    // Node and Link don't inherit from a common "Atom" class to avoid having virtual methods,
    // which couldn't be properly inlined.

    class Node: public HandleTrie::TrieValue {
        public:
        unsigned int count;
        ImportanceType importance;
        Node() {
            count = 1;
            importance = 0.0;
        }
        inline void merge(HandleTrie::TrieValue *other) {
            count += ((Node *) other)->count;
            importance += ((Node *) other)->importance;
        }
    };

    class Edge: public HandleTrie::TrieValue {
        public:
        unsigned int count;
        Edge() {
            count = 1;
        }
        inline void merge(HandleTrie::TrieValue *other) {
            count += ((Edge *) other)->count;
        }
    };

private:

    HandleTrie *nodes;
    HandleTrie *edges;
    ImportanceType tokens_to_distribute;
    mutex tokens_mutex;

};

} // namespace attention_broker_server

#endif // _ATTENTION_BROKER_SERVER_HEBBIANNETWORK_H
