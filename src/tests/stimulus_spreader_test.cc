#include <cstdlib>
#include <cmath>

#include "gtest/gtest.h"
#include "common.pb.h"
#include "attention_broker.grpc.pb.h"
#include "attention_broker.pb.h"
#include "test_utils.h"
#include "expression_hasher.h"
#include "HebbianNetwork.h"
#include "StimulusSpreader.h"

using namespace attention_broker_server;

bool importance_equals(ImportanceType importance, double v2) {
    double v1 = (double) importance;
    return fabs(v2 - v1) < 0.001;
}

TEST(TokenSpreader, distribute_wages) {

    unsigned int num_tests = 10000;
    unsigned int total_nodes = 100;

    TokenSpreader *spreader;
    ImportanceType tokens_to_spread;
    das::HandleCount *update;
    TokenSpreader::StimuliData data;

    for (unsigned int i = 0; i < num_tests; i++) {
        string *handles = build_handle_space(total_nodes);
        spreader = (TokenSpreader *) StimulusSpreader::factory(StimulusSpreaderType::TOKEN);

        tokens_to_spread = 1.0;
        update = new das::HandleCount();
        (*update->mutable_handle_count())[handles[0]] = 2;
        (*update->mutable_handle_count())[handles[1]] = 1;
        (*update->mutable_handle_count())[handles[2]] = 2;
        (*update->mutable_handle_count())[handles[3]] = 1;
        (*update->mutable_handle_count())[handles[4]] = 2;
        (*update->mutable_handle_count())["SUM"] = 8;
        data.importance_changes = new HandleTrie(HANDLE_HASH_SIZE - 1);
        spreader->distribute_wages(update, tokens_to_spread, &data);

        EXPECT_TRUE(importance_equals(((TokenSpreader::ImportanceChanges *) data.importance_changes->lookup(handles[0]))->wages, 0.250));
        EXPECT_TRUE(importance_equals(((TokenSpreader::ImportanceChanges *) data.importance_changes->lookup(handles[1]))->wages, 0.125));
        EXPECT_TRUE(importance_equals(((TokenSpreader::ImportanceChanges *) data.importance_changes->lookup(handles[2]))->wages, 0.250));
        EXPECT_TRUE(importance_equals(((TokenSpreader::ImportanceChanges *) data.importance_changes->lookup(handles[3]))->wages, 0.125));
        EXPECT_TRUE(importance_equals(((TokenSpreader::ImportanceChanges *) data.importance_changes->lookup(handles[4]))->wages, 0.250));
        EXPECT_TRUE(data.importance_changes->lookup(handles[5]) == NULL);
        EXPECT_TRUE(data.importance_changes->lookup(handles[6]) == NULL);
        EXPECT_TRUE(data.importance_changes->lookup(handles[7]) == NULL);
        EXPECT_TRUE(data.importance_changes->lookup(handles[8]) == NULL);
        EXPECT_TRUE(data.importance_changes->lookup(handles[9]) == NULL);

        delete spreader;
    }
}

/*

TEST(TokenSpreader, distribute_wages) {

    unsigned int num_tests = 10000;
    unsigned int total_nodes = 100;

    HebbianNetwork *network;
    TokenSpreader *spreader;
    ImportanceType tokens_to_spread;
    das::HandleCount *update;
    TokenSpreader::StimuliData data;

    for (unsigned int i = 0; i < num_tests; i++) {
        string *handles = build_handle_space(total_nodes);
        network = new HebbianNetwork();
        for (unsigned int j = 0; j < total_nodes; j++) {
            network->add_node(handles[j]);
        }
        spreader = (TokenSpreader *) StimulusSpreader::factory(StimulusSpreaderType::TOKEN);

        tokens_to_spread = 1.0;
        update = new das::HandleCount();
        (*update->mutable_handle_count())[handles[0]] = 2;
        (*update->mutable_handle_count())[handles[1]] = 1;
        (*update->mutable_handle_count())[handles[2]] = 2;
        (*update->mutable_handle_count())[handles[3]] = 1;
        (*update->mutable_handle_count())[handles[4]] = 2;
        (*update->mutable_handle_count())["SUM"] = 8;
        data.importance_changes = new HandleTrie(HANDLE_HASH_SIZE - 1);
        spreader->distribute_wages(update, tokens_to_spread, &data);

        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[0]), 0.25));
        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[1]), 0.125));
        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[2]), 0.25));
        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[3]), 0.125));
        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[4]), 0.25));
        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[5]), 0.0));
        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[6]), 0.0));
        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[7]), 0.0));
        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[8]), 0.0));
        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[9]), 0.0));

        tokens_to_spread = 0.5;
        update = new das::HandleCount();
        (*update->mutable_handle_count())[handles[2]] = 1;
        (*update->mutable_handle_count())[handles[4]] = 2;
        (*update->mutable_handle_count())[handles[6]] = 2;
        (*update->mutable_handle_count())[handles[8]] = 1;
        (*update->mutable_handle_count())["SUM"] = 6;
        data.importance_changes = new HandleTrie(HANDLE_HASH_SIZE - 1);
        spreader->distribute_wages(update, tokens_to_spread, &data);

        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[0]), 0.25));
        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[1]), 0.125));
        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[2]), 0.25 + 0.08333));
        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[3]), 0.125));
        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[4]), 0.25 + 0.16666));
        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[5]), 0.0));
        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[6]), 0.0 + 0.16666));
        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[7]), 0.0));
        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[8]), 0.0 + 0.08333));
        EXPECT_TRUE(importance_equals(network->get_node_importance(handles[9]), 0.0));

        delete network;
        delete spreader;
    }
}

*/
