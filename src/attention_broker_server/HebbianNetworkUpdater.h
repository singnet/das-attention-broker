#ifndef _ATTENTION_BROKER_SERVER_HEBBIANNETWORKUPDATER_H
#define _ATTENTION_BROKER_SERVER_HEBBIANNETWORKUPDATER_H

#include "attention_broker.grpc.pb.h"

using namespace std;

namespace attention_broker_server {

enum class HebbianNetworkUpdaterType {
    EXACT_COUNT
};

/**
 *
 */
class HebbianNetworkUpdater {

public:

    static HebbianNetworkUpdater *factory(HebbianNetworkUpdaterType instance_type);
    virtual ~HebbianNetworkUpdater();

    virtual void correlation(das::HandleList* request) = 0;

protected:

    HebbianNetworkUpdater();

private:

};

class ExactCountHebbianUpdater: public HebbianNetworkUpdater {

public:

    ExactCountHebbianUpdater();
    ~ExactCountHebbianUpdater();

    void correlation(das::HandleList *request);
};

} // namespace attention_broker_server

#endif // _ATTENTION_BROKER_SERVER_HEBBIANNETWORKUPDATER_H
