#ifndef _ATTENTION_BROKER_SERVER_HEBBIANNETWORK_H
#define _ATTENTION_BROKER_SERVER_HEBBIANNETWORK_H

#include <string>
#include <mutex>
#include <forward_list>
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

    unsigned int fan_max;

    // Node and Link don't inherit from a common "Atom" class to avoid having virtual methods,
    // which couldn't be properly inlined.

    class Node: public HandleTrie::TrieValue {
        public:
        unsigned int count;
        ImportanceType importance;
        ImportanceType stimuli_to_spread;
        Node() {
            count = 1;
            importance = 0.0;
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
    Edge *add_edge(string handle1, string handle2, Node *node1, Node *node2);
    Node *lookup_node(string handle);
    Edge *lookup_edge(string handle);
    unsigned int get_node_count(string handle);
    ImportanceType get_node_importance(string handle);
    unsigned int get_edge_count(string handle1, string handle2);
    ImportanceType alienate_tokens();
    void update_nodes(
        bool keep_root_locked,
        bool (*visit_function)(HandleTrie::TrieNode *node, void *data),
        void *data);
    void update_neighbors(
        bool keep_root_locked,
        bool release_root_after_end,
        bool (*visit_function)(
            HandleTrie::TrieNode *node,
            HebbianNetwork::Node *source,
            forward_list<Node *> &targets,
            unsigned int targets_size,
            ImportanceType sum_weights,
            void *data),
        void *data);

private:

    HandleTrie *nodes;
    HandleTrie *edges;
    ImportanceType tokens_to_distribute;
    mutex tokens_mutex;
};

} // namespace attention_broker_server

#endif // _ATTENTION_BROKER_SERVER_HEBBIANNETWORK_H
