#include <string>
#include "HebbianNetworkUpdater.h"
#include "HebbianNetwork.h"
#include "Utils.h"

using namespace attention_broker_server;
using namespace commons;

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

void ExactCountHebbianUpdater::correlation(dasproto::HandleList *request) {
    cout << "XXXXX ExactCountHebbianUpdater::correlation() BEGIN" << endl;

    HebbianNetwork *network = (HebbianNetwork *) request->hebbian_network();
    if (network != NULL) {
        cout << "XXXXX ExactCountHebbianUpdater::correlation() 1" << endl;
        for (const string &s: ((dasproto::HandleList *) request)->list()) {
            cout << "XXXXX ExactCountHebbianUpdater::correlation() 2" << endl;
            cout << "XXXXX " << s << endl;
            cout << "XXXXX ExactCountHebbianUpdater::correlation() 3" << endl;
            network->add_node(s);
        }
        HebbianNetwork::Node *node1;
        HebbianNetwork::Node *node2;
        for (const string &s1: ((dasproto::HandleList *) request)->list()) {
            node1 = network->lookup_node(s1);
            for (const string &s2: ((dasproto::HandleList *) request)->list()) {
                if (s1.compare(s2) < 0) {
                    node2 = network->lookup_node(s2);
                    network->add_symmetric_edge(s1, s2, node1, node2);
                }
            }
        }
    }
}
