#include <cstdlib>

#include "AttentionBrokerServer.h"
#include "Utils.h"
#include "RequestSelector.h"
#include "WorkerThreadPool.h"

using namespace attention_broker_server;
using namespace std;

// --------------------------------------------------------------------------------
// Public methods

WorkerThreadPool::WorkerThreadPool(RequestQueue *stimulus, RequestQueue *correlation) {
    cout << "Building thread pool" << endl;
    stimulus_requests = stimulus;
    correlation_requests = correlation;
    threads_count = AttentionBrokerServer::WORKER_THREADS_COUNT;
    for (unsigned int i = 0; i < threads_count; i++) {
        threads.push_back(new thread(
            &WorkerThreadPool::worker_thread, 
            this, 
            i,
            stimulus_requests,
            correlation_requests));
    }
}

WorkerThreadPool::~WorkerThreadPool() {
}

void WorkerThreadPool::graceful_stop() {
    stop_flag_mutex.lock();
    stop_flag = true;
    stop_flag_mutex.unlock();
    for (thread *worker_thread: threads) {
        worker_thread->join();
    }
}
  
// --------------------------------------------------------------------------------
// Private methods

void WorkerThreadPool::worker_thread(
    unsigned int thread_id, 
    RequestQueue *stimulus_requests, 
    RequestQueue *correlation_requests) {

    cout << "Started thread <" << thread_id << ">" << endl;
    RequestSelector *selector = RequestSelector::factory(
            SelectorType::EVEN_THREAD_COUNT,
            thread_id,
            stimulus_requests,
            correlation_requests);
    pair<RequestType, void *> request;
    bool stop = false;
    while (! stop) {
        request = selector->next();
        if (request.second != NULL) {
            switch (request.first) {
                case RequestType::STIMULUS: {
                    cout << "Thread <" << thread_id << "> processing stimulus request" << endl;
                    break;
                }
                case RequestType::CORRELATION: {
                    cout << "Thread <" << thread_id << "> processing correlation request" << endl;
                    break;
                }
                default: {
                    Utils::error("Invalid request type: " + to_string((int) request.first));
                }
            }
        } else {
            this_thread::sleep_for(chrono::milliseconds(100));
            stop_flag_mutex.lock();
            stop = true;
            stop_flag_mutex.unlock();
        }
    }
}
