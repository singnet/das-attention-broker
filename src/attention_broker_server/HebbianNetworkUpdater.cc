#include <string>
#include "HebbianNetworkUpdater.h"
#include "HebbianNetwork.h"
#include "Utils.h"

using namespace attention_broker_server;

HebbianNetworkUpdater::HebbianNetworkUpdater() {
}

// --------------------------------------------------------------------------------
// Public methods

HebbianNetworkUpdater::~HebbianNetworkUpdater() {
}

HebbianNetworkUpdater *HebbianNetworkUpdater::factory(HebbianNetworkUpdaterType instance_type) {
    switch (instance_type) {
        case HebbianNetworkUpdaterType:: EXACT_COUNT: {
            return new ExactCountHebbianUpdater();
        }
        default: {
            Utils::error("Invalid HebbianNetworkUpdaterType: " + to_string((int) instance_type));
            return NULL; // to avoid warnings
        }
    }

}

ExactCountHebbianUpdater::ExactCountHebbianUpdater() {
}

ExactCountHebbianUpdater::~ExactCountHebbianUpdater() {
}

void ExactCountHebbianUpdater::correlation(das::HandleList *request) {
    HebbianNetwork *network = (HebbianNetwork *) request->hebbian_network();
    if (network != NULL) {
        for (const string &s1: ((das::HandleList *) request)->handle_list()) {
            network->add_node(s1);
            for (const string &s2: ((das::HandleList *) request)->handle_list()) {
                if (s1.compare(s2) < 0) {
                    network->add_symmetric_edge(s1, s2);
                }
            }
        }
    }
}
