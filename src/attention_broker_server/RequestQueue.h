#ifndef _ATTENTION_BROKER_SERVER_REQUESTQUEUE_H
#define _ATTENTION_BROKER_SERVER_REQUESTQUEUE_H

#include <unordered_map>
#include <mutex>

namespace attention_broker_server {

class RequestQueue {

public:

    RequestQueue(unsigned int initial_size);
    ~RequestQueue();

    void enqueue(void *request);

protected:

    void *dequeue();
    unsigned int current_size();
    unsigned int current_start();
    unsigned int current_end();

private:

    std::mutex request_queue_mutex;

    void **requests; // GRPC documentation states that request types should not be inherited
    unsigned int size;
    unsigned int count;
    unsigned int start;
    unsigned int end;

    void enlarge_request_queue();
};

}

#endif // _ATTENTION_BROKER_SERVER_REQUESTQUEUE_H
