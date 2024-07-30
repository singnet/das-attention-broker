#ifndef _ATTENTION_BROKER_SERVER_REQUESTQUEUE_H
#define _ATTENTION_BROKER_SERVER_REQUESTQUEUE_H

#include <mutex>

namespace attention_broker_server {

/**
 * Data abstraction of a synchronized (thread-safe) queue for AttentionBrokerServer requests.
 *
 * Internally, this abstraction uses an array of requests to avoid the need to create cell
 * objects on every insertion. Because of this, on new insertions it's possible to reach queue
 * size limit during an insertion. When that happens, the array is doubled in size. Initial size
 * is passed as a constructor's parameter.
 */
class RequestQueue {

public:

    RequestQueue(); /// Basic constructor which uses a parameter from AttentionBrokerServer as initial size.
    RequestQueue(unsigned int size); // Constructor which uses the passed initial size.
    ~RequestQueue(); /// Destructor.

    /**
     * Enqueues a request.
     *
     * @param request Request to be queued.
     */
    void enqueue(void *request);

    /**
     * Dequeues a request.
     *
     * @return The dequeued request.
     */
    void *dequeue();

protected:

    unsigned int current_size();
    unsigned int current_start();
    unsigned int current_end();
    unsigned int current_count();

private:

    std::mutex request_queue_mutex;

    void **requests; // GRPC documentation states that request types should not be inherited
    unsigned int size;
    unsigned int count;
    unsigned int start;
    unsigned int end;

    void enlarge_request_queue();
};

} // namespace attention_broker_server

#endif // _ATTENTION_BROKER_SERVER_REQUESTQUEUE_H
