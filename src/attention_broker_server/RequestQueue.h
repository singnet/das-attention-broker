#ifndef _ATTENTION_BROKER_SERVER_REQUESTQUEUE_H
#define _ATTENTION_BROKER_SERVER_REQUESTQUEUE_H

#include <unordered_map>
#include <mutex>

namespace attention_broker_server {

class RequestQueue {

public:

    RequestQueue();
    RequestQueue(unsigned int size);
    ~RequestQueue();

    void enqueue(void *request);
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
