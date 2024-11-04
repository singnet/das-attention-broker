#ifndef _QUERY_ELEMENT_QUERYELEMENT_H
#define _QUERY_ELEMENT_QUERYELEMENT_H

#include <string>
#include <memory>
#include "Utils.h"
#include "QueryNode.h"

using namespace std;
using namespace query_node;
using namespace commons;

namespace query_element {

/**
 *
 */
class QueryElement {

public:

    string id;
    string subsequent_id;

    QueryElement();
    virtual ~QueryElement();

    virtual bool is_terminal() { return false; }
    virtual void setup_buffers() = 0;
    virtual void graceful_shutdown() = 0;

};

} // namespace query_element

#endif // _QUERY_ELEMENT_QUERYELEMENT_H
