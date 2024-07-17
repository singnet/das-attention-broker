#include "expression_hasher.h"
#include "StimulusSpreader.h"
#include "HebbianNetwork.h"
#include "AttentionBrokerServer.h"
#include "Utils.h"
#include <string>
#include <forward_list>

using namespace attention_broker_server;

StimulusSpreader::StimulusSpreader() {
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

    ImportanceType to_spread = source->stimuli_to_spread;

    ImportanceType count_source = source->count;
    for (auto target: targets) {
        ImportanceType count_source_target = ((HebbianNetwork::Edge *) node->value)->count;
        ImportanceType w = count_source_target / count_source;
        ImportanceType stimulus = (w * to_spread) / sum_weights;
        target->importance += stimulus;
    }
    return false;
}

// ------------------------------------------------

void TokenSpreader::distribute_wages(
    das::HandleCount *handle_count,
    ImportanceType &total_to_spread,
    DATA *data) {

    auto iterator = handle_count->handle_count().find("SUM");
    unsigned int total_wages = iterator->second;
    for (auto pair: handle_count->handle_count()) {
        if (pair.first != "SUM") {
            double normalized_amount = (((double) pair.second) * total_to_spread) / total_wages;
            data->importance_changes->insert(pair.first, new TokenSpreader::ImportanceChanges(0.0, normalized_amount));
        }
    }
}

void TokenSpreader::spread_stimuli(das::HandleCount *request) {

    HebbianNetwork *network = (HebbianNetwork *) request->hebbian_network();
    if (network == NULL) {
        return;
    }

    DATA data;
    data.importance_changes = new HandleTrie(HANDLE_HASH_SIZE - 1);
    data.rent_rate = AttentionBrokerServer::RENT_RATE;

    // Collect rent
    network->update_nodes(true, &collect_rent, (void *) &data);
    // Distribute wages
    ImportanceType total_to_spread = network->alienate_tokens();
    total_to_spread += data.total_rent;
    distribute_wages(request, total_to_spread, &data);

    // Consolidate changes
    network->update_nodes(true, &consolidate_rent_and_wages, (void *) &data);

    // Spread activation (1 cycle)
    network->update_neighbors(true, false, &compute_stimulus, &data);
    network->update_neighbors(true, true, &consolidate_stimulus, &data);
}
