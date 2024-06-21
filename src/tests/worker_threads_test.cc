#include <cstdlib>

#include "gtest/gtest.h"
#include "common.pb.h"
#include "attention_broker.grpc.pb.h"
#include "attention_broker.pb.h"

#include "Utils.h"
#include "RequestQueue.h"
#include "WorkerThreads.h"

using namespace attention_broker_server;

class TestRequestQueue: public RequestQueue {
    public:
        TestRequestQueue() : RequestQueue() {
        }
        unsigned int test_current_count() {
            return current_count();
        }
};


TEST(WorkerThreads, Basics) {
    
    das::HandleCount *handle_count;
    das::HandleList *handle_list;

    for (double stimulus_prob: {0.0, 0.25, 0.5, 0.75, 1.0}) {
        TestRequestQueue *stimulus = new TestRequestQueue();
        TestRequestQueue *correlation = new TestRequestQueue();
        WorkerThreads *pool = new WorkerThreads(stimulus, correlation);
        for (int i = 0; i < 100000; i++) {
            if (Utils::flip_coin(stimulus_prob)) {
                handle_count = new das::HandleCount();
                stimulus->enqueue(handle_count);
            } else {
                handle_list = new das::HandleList();
                correlation->enqueue(handle_list);
            }
        }
        this_thread::sleep_for(chrono::milliseconds(3000));
        EXPECT_TRUE(stimulus->test_current_count() == 0);
        EXPECT_TRUE(correlation->test_current_count() == 0);
        pool->graceful_stop();
        delete pool;
        delete stimulus;
        delete correlation;
    }
}
