#ifndef _QUERY_ELEMENT_REMOTEITERATOR_H
#define _QUERY_ELEMENT_REMOTEITERATOR_H

#include "Source.h"

using namespace std;

namespace query_element {

/**
 *
 */
class RemoteIterator : public QueryElement {

public:
    
    RemoteIterator(const string &local_id);
    ~RemoteIterator();
    virtual void graceful_shutdown();
    virtual void setup_buffers();
    bool finished();
    DASQueryAnswer *pop();

private:

    shared_ptr<QueryNode> remote_input_buffer;
    string local_id;
};

} // namespace query_element

#endif // _QUERY_ELEMENT_REMOTEITERATOR_H
