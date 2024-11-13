#ifndef _QUERY_ENGINE_DASNODE_H
#define _QUERY_ENGINE_DASNODE_H

#include "StarNode.h"

using namespace std;

namespace query_engine {

/**
 *
 */
class DASNode : public StarNode {

public:

    DASNode(const string &node_id);
    DASNode(const string &node_id, const string &server_id);
    ~DASNode();

private:

};

} // namespace query_engine

#endif // _QUERY_ENGINE_DASNODE_H
