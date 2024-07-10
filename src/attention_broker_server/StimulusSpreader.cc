#include "expression_hasher.h"
#include "StimulusSpreader.h"
#include "HebbianNetwork.h"
#include "Utils.h"
#include <string>

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

static bool deposit_wages(HandleTrie::TrieNode *node, void *wages) {
    TokenSpreader::DoubleValue *wages_value = (TokenSpreader::DoubleValue *) ((HandleTrie *) wages)->lookup(node->suffix);
    if (wages_value != NULL) {
        ((HebbianNetwork::Node *) node->value)->importance += wages_value->value;
    }
    return false;
}

void TokenSpreader::distribute_wages(HebbianNetwork *network, das::HandleCount *handle_count, ImportanceType &tokens_to_spread) {
    HandleTrie wages(HANDLE_HASH_SIZE - 1);
    auto iterator = handle_count->handle_count().find("SUM");
    unsigned int total_wages = iterator->second;
    for (auto pair: handle_count->handle_count()) {
        if (pair.first != "SUM") {
            double normalized_amount = (((double) pair.second) * tokens_to_spread) / total_wages;
            wages.insert(pair.first, new DoubleValue(normalized_amount));
        }
    }
    network->update_nodes(&deposit_wages, &wages);
}

void TokenSpreader::spread_activation() {
    
}

void TokenSpreader::spread_stimuli(das::HandleCount *request) {
    HebbianNetwork *network = (HebbianNetwork *) request->hebbian_network();
    if (network != NULL) {
        ImportanceType tokens_to_spread = network->alienate_tokens();
        if (tokens_to_spread == 0.0) {
            //tokens_to_spread = collect_rent();
        }
        distribute_wages(network, request, tokens_to_spread);
        spread_activation();
    }
}
