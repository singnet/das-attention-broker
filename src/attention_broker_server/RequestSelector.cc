#include "RequestSelector.h"
#include "Utils.h"
#include <string>

using namespace attention_broker_server;

// --------------------------------------------------------------------------------
// Public methods

RequestSelector::RequestSelector(
    unsigned int thread_id, 
    RequestQueue *stimulus, 
    RequestQueue *correlation) {

    this->thread_id = thread_id;
    this->stimulus = stimulus;
    this->correlation = correlation;
}

RequestSelector::~RequestSelector() {
}

EvenThreadCount::EvenThreadCount(
    unsigned int thread_id,
    RequestQueue *stimulus, 
    RequestQueue *correlation) : RequestSelector(thread_id, stimulus, correlation) {

    even_thread_id = ((thread_id % 2) == 0);
}

RequestSelector *RequestSelector::factory(
    SelectorType selector_type, 
    unsigned int thread_id, 
    RequestQueue *stimulus, 
    RequestQueue *correlation) {

    switch (selector_type) {
        case SelectorType::EVEN_THREAD_COUNT: {
            return new EvenThreadCount(thread_id, stimulus, correlation);
        }
        default: {
            Utils::error("Invalid selector type: " + to_string((int) selector_type));
            return NULL; // to avoid warnings
        }
    }
}

pair<RequestType, void *> EvenThreadCount::next() {
    pair<RequestType, void *> answer;
    if (even_thread_id) {
        answer.first = RequestType::STIMULUS;
        answer.second = (void *) stimulus->dequeue();
    } else {
        answer.first = RequestType::CORRELATION;
        answer.second = (void *) correlation->dequeue();
    }
    return answer;
}
