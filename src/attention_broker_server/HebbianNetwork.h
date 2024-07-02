#ifndef _ATTENTION_BROKER_SERVER_HEBBIANNETWORK_H
#define _ATTENTION_BROKER_SERVER_HEBBIANNETWORK_H

#include <string>
#include "HandleTrie.h"

using namespace std;

namespace attention_broker_server {

/**
 *
 */
class HebbianNetwork {

public:

    HebbianNetwork();
    ~HebbianNetwork();

    void add_node(string handle);
    void add_symmetric_edge(string handle1, string handle2);
    unsigned int get_node_count(string handle);
    unsigned int get_symmetric_edge_count(string handle1, string handle2);

private:

    HandleTrie *node_count;
    HandleTrie *symmetric_edge_count;

    class AccumulatorValue: public HandleTrie::TrieValue {
        public:
        unsigned int count;
        AccumulatorValue() {
            this->count = 1;
        }
        void merge(TrieValue *other) {
            count += ((AccumulatorValue *) other)->count;
        }
    };
};

} // namespace attention_broker_server

#endif // _ATTENTION_BROKER_SERVER_HEBBIANNETWORK_H
