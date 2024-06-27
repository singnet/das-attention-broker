#ifndef _ATTENTION_BROKER_SERVER_WORKERTHREADS_H
#define _ATTENTION_BROKER_SERVER_WORKERTHREADS_H

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>

#include "RequestQueue.h"

using namespace std;

namespace attention_broker_server {

/**
 *
 */
class WorkerThreads {

public:

    WorkerThreads(RequestQueue *stimulus, RequestQueue *correlation);
    ~WorkerThreads();

    void graceful_stop();

protected:


private:

    unsigned int threads_count;
    vector<thread *> threads;
    bool stop_flag = false;
    RequestQueue *stimulus_requests;
    RequestQueue *correlation_requests;
    mutex stop_flag_mutex;

    void worker_thread(
        unsigned int thread_id, 
        RequestQueue *stimulus_requests, 
        RequestQueue *correlation_requests);
};

} // namespace attention_broker_server

#endif // _ATTENTION_BROKER_SERVER_WORKERTHREADS_H
