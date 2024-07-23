#include "expression_hasher.h"
#include "StimulusSpreader.h"
#include "HebbianNetwork.h"
#include "AttentionBrokerServer.h"
#include "Utils.h"
#include <string>
#include <forward_list>

using namespace attention_broker_server;

StimulusSpreader::StimulusSpreader() {
    fan_max = 1;
}

// --------------------------------------------------------------------------------
// Public methods

StimulusSpreader::~StimulusSpreader() {
}

StimulusSpreader *StimulusSpreader::factory(StimulusSpreaderType instance_type) {
    switch (instance_type) {
        case StimulusSpreaderType::TOKEN : {
            return new TokenSpreader();
        }
        default: {
            Utils::error("Invalid StimulusSpreaderType: " + to_string((int) instance_type));
            return NULL; // to avoid warnings
        }
    }

}

TokenSpreader:: TokenSpreader() {
}

TokenSpreader:: ~TokenSpreader() {
}

// ------------------------------------------------
// "visit" functions used to traverse network

typedef TokenSpreader::StimuliData DATA;

static bool collect_rent(HandleTrie::TrieNode *node, void *data) {
    ImportanceType rent = ((DATA *) data)->rent_rate * \
                          ((HebbianNetwork::Node *) node->value)->importance;
    ((DATA *) data)->total_rent += rent;
    ImportanceType wages = 0.0;
    ((DATA *) data)->importance_changes->insert(
        node->suffix,
        new TokenSpreader::ImportanceChanges(rent, wages));
    return false;
}

static bool consolidate_rent_and_wages(HandleTrie::TrieNode *node, void *data) {
    TokenSpreader::ImportanceChanges *changes = (TokenSpreader::ImportanceChanges *) ((DATA *) data)->importance_changes->lookup(node->suffix);
    ((HebbianNetwork::Node *) node->value)->importance -= changes->rent;
    ((HebbianNetwork::Node *) node->value)->importance += changes->wages;
    return false;
}

static bool compute_stimulus(
    HandleTrie::TrieNode *node,
    HebbianNetwork::Node *source,
    forward_list<HebbianNetwork::Node *> &targets,
    unsigned int targets_size,
    ImportanceType sum_weights,
    void *data) {

    ImportanceType fan_ratio = (double) targets_size / ((DATA *) data)->fan_max;
    ImportanceType spreading_rate = ((DATA *) data)->spreading_rate_lowerbound + \
                                    (((DATA *) data)->spreading_rate_range_size * \
                                     fan_ratio);

    ImportanceType to_spread = source->importance * spreading_rate;
    source->importance -= to_spread;
    source->stimuli_to_spread = to_spread;

    return false;
}

static bool consolidate_stimulus(
    HandleTrie::TrieNode *node,
    HebbianNetwork::Node *source,
    forward_list<HebbianNetwork::Node *> &targets,
    unsigned int targets_size,
    ImportanceType sum_weights,
    void *data) {

    cout << "consolidate_stimulus() BEGIN" << endl;
    cout << "node: " << node->to_string() << endl;
    cout << "source: " << source->to_string() << endl;
    cout << "targets_size: " << targets_size << endl;
    cout << "sum_weights: " << sum_weights << endl;
    ImportanceType to_spread = source->stimuli_to_spread;

    ImportanceType count_source = source->count;
    cout << "count_source: " << count_source << endl;
    for (auto target: targets) {
        cout << "target: " << source->to_string() << endl;
        cout << "((HebbianNetwork::Edge *) node->value) " << ((HebbianNetwork::Edge *) node->value) << endl;
        cout << "((HebbianNetwork::Edge *) node->value)->count " << ((HebbianNetwork::Edge *) node->value)->count << endl;
        ImportanceType count_source_target = ((HebbianNetwork::Edge *) node->value)->count;
        cout << "count_source_target: " << count_source_target << endl;
        ImportanceType w = count_source_target / count_source;
        cout << "w: " << w << endl;
        ImportanceType stimulus = (w * to_spread) / sum_weights;
        cout << "stimulus: " << stimulus << endl;
        cout << "target->importance (before): " << target->importance << endl;
        target->importance += stimulus;
        cout << "target->importance  (after): " << target->importance << endl;
    }
    cout << "consolidate_stimulus() END" << endl;
    return false;
}

static bool update_fan_max(
    HandleTrie::TrieNode *node,
    HebbianNetwork::Node *source,
    forward_list<HebbianNetwork::Node *> &targets,
    unsigned int targets_size,
    ImportanceType sum_weights,
    void *data) {

    // Do nothing. Update is carried out in the traversing method.
    cout << "XXX targets_size: " << targets_size << endl;
    cout << "source: " << source->to_string() << endl;
    for (auto target: targets) {
        cout << "\ttarget: " << target->to_string() << endl;
    }
    return false;
}

// ------------------------------------------------

void TokenSpreader::distribute_wages(
    das::HandleCount *handle_count,
    ImportanceType &total_to_spread,
    DATA *data) {

    auto iterator = handle_count->handle_count().find("SUM");
    if (iterator == handle_count->handle_count().end()) {
        Utils::error("Missing 'SUM' key in HandleCount request");
    }
    unsigned int total_wages = iterator->second;
    for (auto pair: handle_count->handle_count()) {
        if (pair.first != "SUM") {
            double normalized_amount = (((double) pair.second) * total_to_spread) / total_wages;
            data->importance_changes->insert(pair.first, new TokenSpreader::ImportanceChanges(0.0, normalized_amount));
        }
    }
}

void TokenSpreader::spread_stimuli(das::HandleCount *request) {

    //cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
    //cout << "XXX 1" << endl;
    HebbianNetwork *network = (HebbianNetwork *) request->hebbian_network();
    if (network == NULL) {
        return;
    }
    cout << "XXX 2" << endl;
    if (network->fan_max == 0) {
        cout << "XXX 2.a" << endl;
        network->update_neighbors(true, true, &update_fan_max, NULL);
    }

    DATA data;
    data.importance_changes = new HandleTrie(HANDLE_HASH_SIZE - 1);
    data.rent_rate = AttentionBrokerServer::RENT_RATE;
    data.spreading_rate_lowerbound = AttentionBrokerServer::SPREADING_RATE_LOWERBOUND;
    data.spreading_rate_range_size = 
        AttentionBrokerServer::SPREADING_RATE_UPPERBOUND - AttentionBrokerServer::SPREADING_RATE_LOWERBOUND;
    data.fan_max = network->fan_max;
    cout << "XXX data.fan_max: " << data.fan_max << endl;
    return;

    // Collect rent
    cout << "XXX 3" << endl;
    network->update_nodes(true, &collect_rent, (void *) &data);
    cout << "XXX 4" << endl;
    // Distribute wages
    ImportanceType total_to_spread = network->alienate_tokens();
    cout << "XXX 5" << endl;
    total_to_spread += data.total_rent;
    cout << "XXX 6" << endl;
    cout << "XXX total_to_spread: " << total_to_spread << endl;
    distribute_wages(request, total_to_spread, &data);
    cout << "XXX 7" << endl;

    // Consolidate changes
    network->update_nodes(true, &consolidate_rent_and_wages, (void *) &data);
    cout << "XXX 8" << endl;

    // Spread activation (1 cycle)
    network->update_neighbors(true, true, &compute_stimulus, &data);
    cout << "XXX 9" << endl;
    network->update_neighbors(true, true, &consolidate_stimulus, &data);
    cout << "XXX 10" << endl;
}
