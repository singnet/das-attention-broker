#ifndef _ATTENTION_BROKER_SERVER_HEBBIANNETWORK_H
#define _ATTENTION_BROKER_SERVER_HEBBIANNETWORK_H

#include <string>
#include <mutex>
#include <forward_list>
#include "HandleTrie.h"
#include "expression_hasher.h"

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

    unsigned int largest_arity;
    mutex largest_arity_mutex;

    // Node and Link don't inherit from a common "Atom" class to avoid having virtual methods,
    // which couldn't be properly inlined.

    class Node: public HandleTrie::TrieValue {
        public:
        unsigned int arity;
        unsigned int count;
        ImportanceType importance;
        ImportanceType stimuli_to_spread;
        HandleTrie *neighbors;
        Node() {
            arity = 0;
            count = 1;
            importance = 0.0;
            stimuli_to_spread = 0.0;
            neighbors = new HandleTrie(HANDLE_HASH_SIZE - 1);
        }
        inline void merge(HandleTrie::TrieValue *other) {
            count += ((Node *) other)->count;
            importance += ((Node *) other)->importance;
        }
        string to_string();
    };

    class Edge: public HandleTrie::TrieValue {
        public:
        unsigned int count;
        Node *node1;
        Node *node2;
        Edge() {
            count = 1;
            node1 = node2 = NULL;
        }
        inline void merge(HandleTrie::TrieValue *other) {
            count += ((Edge *) other)->count;
        }
        string to_string();
    };

    Node *add_node(string handle);
    Edge *add_asymmetric_edge(string handle1, string handle2, Node *node1, Node *node2);
    void add_symmetric_edge(string handle1, string handle2, Node *node1, Node *node2);
    Node *lookup_node(string handle);
    unsigned int get_node_count(string handle);
    ImportanceType get_node_importance(string handle);
    unsigned int get_asymmetric_edge_count(string handle1, string handle2);
    ImportanceType alienate_tokens();
    void visit_nodes(
        bool keep_root_locked,
        bool (*visit_function)(HandleTrie::TrieNode *node, void *data),
        void *data);

private:

    HandleTrie *nodes;
    HandleTrie *edges;
    ImportanceType tokens_to_distribute;
    mutex tokens_mutex;
};

} // namespace attention_broker_server

#endif // _ATTENTION_BROKER_SERVER_HEBBIANNETWORK_H
