#ifndef _QUERY_ELEMENT_SINK_H
#define _QUERY_ELEMENT_SINK_H

#include "QueryElement.h"

using namespace std;

namespace query_element {

/**
 *
 */
class Sink : public QueryElement {

public:

    Sink(QueryElement *precedent, const string &id);
    virtual ~Sink();

    virtual void setup_buffers();
    virtual void graceful_shutdown();

protected:

    shared_ptr<QueryNode> input_buffer;
    QueryElement *precedent;
};

} // namespace query_element

#endif // _QUERY_ELEMENT_SINK_H
