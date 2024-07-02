#ifndef _ATTENTION_BROKER_SERVER_REQUESTSELECTOR_H
#define _ATTENTION_BROKER_SERVER_REQUESTSELECTOR_H

#include "HebbianNetwork.h"
#include "RequestQueue.h"

namespace attention_broker_server {
using namespace std;

enum class SelectorType {
    EVEN_THREAD_COUNT
};

enum class RequestType {
    STIMULUS, 
    CORRELATION
};

/**
 *
 */
class RequestSelector {

public:

    virtual ~RequestSelector();

    static RequestSelector *factory(
        SelectorType selector_type, 
        unsigned int thread_id, 
        RequestQueue *stimulus, 
        RequestQueue *correlation);

    virtual pair<RequestType, void *> next() = 0;

protected:

    RequestSelector(unsigned int thread_id, RequestQueue *stimulus, RequestQueue *correlation);
    unsigned int thread_id;
    RequestQueue *stimulus;
    RequestQueue *correlation;
};

class EvenThreadCount : public RequestSelector {

public:

    ~EvenThreadCount();

    EvenThreadCount(unsigned int thread_id, RequestQueue *stimulus, RequestQueue *correlation);
    pair<RequestType, void *> next();

private:

    bool even_thread_id;
};

} // namespace attention_broker_server

#endif // _ATTENTION_BROKER_SERVER_REQUESTSELECTOR_H
