#ifndef _QUERY_ELEMENT_REMOTESINK_H
#define _QUERY_ELEMENT_REMOTESINK_H

#include "Sink.h"

using namespace std;

namespace query_element {

/**
 * A special sink which forwards the query results to a remote QueryElement (e.g. a RemoteIterator).
 */
class RemoteSink: public Sink {

public:

    /**
     * Constructor.
     *
     * @param precedent QueryElement just below in the query tree.
     * @param local_id ID of this element in the network connecting to the remote
     *        peer (typically "host:port").
     * @param remote_id network ID of the remote peer (typically "host:port").
     * @param delete_precedent_on_destructor If true, the destructor of this QueryElement will
     *        also destruct the passed precedent QueryElement (defaulted to false).
     */
    RemoteSink(
        QueryElement *precedent, 
        const string &local_id,
        const string &remote_id,
        bool delete_precedent_on_destructor = false);

    /**
     * Destructor.
     */
    ~RemoteSink();

    // --------------------------------------------------------------------------------------------
    // QueryElement API

    /**
     * Setups QueryNode elements related to the communication with the remote element.
     */
    virtual void setup_buffers();

    /**
     * Gracefully shuts down the queue processor thread and the remote communication QueryNodes
     * present in this QueryElement.
     */
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
