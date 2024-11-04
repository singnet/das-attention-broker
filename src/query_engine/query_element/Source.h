#ifndef _QUERY_ELEMENT_SOURCE_H
#define _QUERY_ELEMENT_SOURCE_H

#include "QueryElement.h"

using namespace std;

namespace query_element {

/**
 *
 */
class Source : public QueryElement {

public:

    static string DEFAULT_ATTENTION_BROKER_PORT;

    Source(const string &attention_broker_address);
    Source();
    virtual ~Source();

    virtual void graceful_shutdown();
    virtual void setup_buffers();
    virtual void query_answers_finished();

protected:

    string attention_broker_address;
    shared_ptr<QueryNode> output_buffer;
    QueryElement *subsequent;
};

} // namespace query_element

#endif // _QUERY_ELEMENT_SOURCE_H
