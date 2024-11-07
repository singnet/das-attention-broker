#ifndef _QUERY_ELEMENT_REMOTESINK_H
#define _QUERY_ELEMENT_REMOTESINK_H

#include "Sink.h"

using namespace std;

namespace query_element {

/**
 *
 */
class RemoteSink: public Sink {

public:

    RemoteSink(
        QueryElement *precedent, 
        const string &local_id,
        const string &remote_id,
        bool delete_precedent_on_destructor = false);
    ~RemoteSink();
    virtual void setup_buffers();
    virtual void graceful_shutdown();

private:

    shared_ptr<QueryNode> remote_output_buffer;
    string local_id;
    string remote_id;
    thread *queue_processor;

    void queue_processor_method();
};

} // namespace query_element

#endif // _QUERY_ELEMENT_REMOTESINK_H
