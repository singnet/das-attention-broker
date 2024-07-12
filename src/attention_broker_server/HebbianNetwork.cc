#include <iostream>
#include <stack>
#include "HebbianNetwork.h"
#include "expression_hasher.h"


using namespace attention_broker_server;

// --------------------------------------------------------------------------------
// Public methods

HebbianNetwork::HebbianNetwork() {
    nodes = new HandleTrie(HANDLE_HASH_SIZE - 1);
    edges = new HandleTrie(2 * (HANDLE_HASH_SIZE - 1));
    tokens_mutex.lock();
    tokens_to_distribute = 1.0;
    tokens_mutex.unlock();
}

HebbianNetwork::~HebbianNetwork() {
    delete nodes;
    delete edges;
}

string HebbianNetwork::Node::to_string() {
    return "(" + std::to_string(count) + ", " + std::to_string(importance) + ")";
}

string HebbianNetwork::Edge::to_string() {
    return "(" + std::to_string(count) + ")";
}

HebbianNetwork::Node *HebbianNetwork::add_node(string handle) {
    return (Node *) nodes->insert(handle, new HebbianNetwork::Node());
}

HebbianNetwork::Edge *HebbianNetwork::add_edge(string handle1, string handle2, Node *node1, Node *node2) {
    string composite = handle1 + handle2;
    Edge *edge = (Edge *) edges->insert(composite, new HebbianNetwork::Edge());
    edge->node1 = node1;
    edge->node2 = node2;
    return edge;
}

HebbianNetwork::Node *HebbianNetwork::lookup_node(string handle) {
    return (Node *) nodes->lookup(handle);
}

HebbianNetwork::Edge *HebbianNetwork::lookup_edge(string handle) {
    return (Edge *) edges->lookup(handle);
}

unsigned int HebbianNetwork::get_node_count(string handle) {
    Node *node = (Node *) nodes->lookup(handle);
    if (node == NULL) {
        return 0;
    } else {
        return node->count;
    }
}

ImportanceType HebbianNetwork::get_node_importance(string handle) {
    Node *node = (Node *) nodes->lookup(handle);
    if (node == NULL) {
        return 0;
    } else {
        return node->importance;
    }
}

unsigned int HebbianNetwork::get_edge_count(string handle1, string handle2) {
    Edge *edge;
    string composite;
    if (handle1.compare(handle2) < 0) {
        composite = handle1 + handle2;
    } else {
        composite = handle2 + handle1;
    }
    edge = (Edge *) edges->lookup(composite);
    if (edge == NULL) {
        return 0;
    } else {
        return edge->count;
    }
}

ImportanceType HebbianNetwork::alienate_tokens() {
    ImportanceType answer;
    tokens_mutex.lock();
    answer = tokens_to_distribute;
    tokens_to_distribute = 0.0;
    tokens_mutex.unlock();
    return answer;
}

void HebbianNetwork::update_nodes(
    bool keep_root_locked,
    bool (*visit_function)(HandleTrie::TrieNode *node, void *data),
    void *data) {

    nodes->traverse(keep_root_locked, visit_function, data);
}


static inline void release_locks(
    HandleTrie::TrieNode *root,
    HandleTrie::TrieNode *cursor,
    bool keep_root_locked) {

    if (keep_root_locked && (root != cursor)) {
        root->trie_node_mutex.unlock();
    }
    cursor->trie_node_mutex.unlock();
}

void HebbianNetwork::update_neighbors(
    bool keep_root_locked,
    bool (*visit_function)(
        HebbianNetwork::Node *,
        forward_list<HebbianNetwork::Node *> &targets,
        void *data),
    void *data) {

    HandleTrie::TrieNode *root = edges->root;
    std::stack<tuple<bool, unsigned int, HandleTrie::TrieNode *>> stack;
    HandleTrie::TrieNode *cursor = NULL;
    HebbianNetwork::Edge *edge_value = NULL;
    HebbianNetwork::Node *source = NULL;
    forward_list<HebbianNetwork::Node *> targets;
    unsigned int level;
    bool second_handle_flag = false;
    unsigned int target_threshold = HANDLE_HASH_SIZE - 1;
    enum State {LOOKING_FOR_FIRST_HANDLE, LOOKING_FOR_SECOND_HANDLE};
    State current_state = LOOKING_FOR_FIRST_HANDLE;

    stack.push(make_tuple(false, 0, root));

    while (! stack.empty()) {
        second_handle_flag = get<0>(stack.top());
        level = get<1>(stack.top());
        cursor = get<2>(stack.top());
        stack.pop();
        cursor->trie_node_mutex.lock();

        if ((current_state == LOOKING_FOR_SECOND_HANDLE) && ! second_handle_flag) {
            if (visit_function(source, targets, data)) {
                release_locks(root, cursor, keep_root_locked);
                return;
            }
            current_state = LOOKING_FOR_FIRST_HANDLE;
            targets.clear();
        }
        if (cursor->suffix_start > 0) {
            if (current_state == LOOKING_FOR_FIRST_HANDLE) {
                edge_value = (HebbianNetwork::Edge *) cursor->value;
                source = edge_value->node1;
                targets.push_front(edge_value->node2);
                if (visit_function(source, targets, data)) {
                    release_locks(root, cursor, keep_root_locked);
                    return;
                }
                targets.clear();
            } else {
                // current_state == LOOKING_FOR_SECOND_HANDLE
                edge_value = (HebbianNetwork::Edge *) cursor->value;
                source = edge_value->node1;
                targets.push_front(edge_value->node2);
            }
        } else {
            unsigned int deeper_level = level + 1;
            if (deeper_level == target_threshold) {
                current_state = LOOKING_FOR_SECOND_HANDLE;
                second_handle_flag = true;
            }
            for (unsigned int i = TRIE_ALPHABET_SIZE - 1; ; i--) {
                if (cursor->children[i] != NULL) {
                    stack.push(make_tuple(second_handle_flag, deeper_level, cursor->children[i]));
                }
                if (i == 0) {
                    break;
                }
            }
        }
        if ((! keep_root_locked) || (cursor != root)) {
            cursor->trie_node_mutex.unlock();
        }
    }
    if (current_state == LOOKING_FOR_SECOND_HANDLE) {
        visit_function(source, targets, data);
    }
    release_locks(root, cursor, keep_root_locked);
    if (keep_root_locked) {
        root->trie_node_mutex.unlock();
    }
}
