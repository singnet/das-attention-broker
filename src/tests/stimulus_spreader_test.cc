#include <cstdlib>
#include <cmath>

#include "gtest/gtest.h"
#include "common.pb.h"
#include "attention_broker.grpc.pb.h"
#include "attention_broker.pb.h"
#include "test_utils.h"
#include "expression_hasher.h"
#include "HebbianNetwork.h"
#include "HebbianNetworkUpdater.h"
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
    das::HandleCount *request;
    TokenSpreader::StimuliData data;

    for (unsigned int i = 0; i < num_tests; i++) {
        string *handles = build_handle_space(total_nodes);
        spreader = (TokenSpreader *) StimulusSpreader::factory(StimulusSpreaderType::TOKEN);

        tokens_to_spread = 1.0;
        request = new das::HandleCount();
        (*request->mutable_handle_count())[handles[0]] = 2;
        (*request->mutable_handle_count())[handles[1]] = 1;
        (*request->mutable_handle_count())[handles[2]] = 2;
        (*request->mutable_handle_count())[handles[3]] = 1;
        (*request->mutable_handle_count())[handles[4]] = 2;
        (*request->mutable_handle_count())["SUM"] = 8;
        data.importance_changes = new HandleTrie(HANDLE_HASH_SIZE - 1);
        spreader->distribute_wages(request, tokens_to_spread, &data);

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

static HebbianNetwork *build_test_network(string *handles) {

    HebbianNetwork *network = new HebbianNetwork();
    das::HandleList *request;
    ExactCountHebbianUpdater *updater = \
        (ExactCountHebbianUpdater *) HebbianNetworkUpdater::factory(HebbianNetworkUpdaterType::EXACT_COUNT);

    request = new das::HandleList();
    request->set_hebbian_network((unsigned long) network);
    request->add_handle_list(handles[0]);
    request->add_handle_list(handles[1]);
    request->add_handle_list(handles[2]);
    request->add_handle_list(handles[3]);
    updater->correlation(request);

    request = new das::HandleList();
    request->set_hebbian_network((unsigned long) network);
    request->add_handle_list(handles[1]);
    request->add_handle_list(handles[2]);
    request->add_handle_list(handles[4]);
    request->add_handle_list(handles[5]);
    updater->correlation(request);

    return network;
}

TEST(TokenSpreader, spread_stimuli) {

    string *handles = build_handle_space(6, true);
    for (unsigned int i = 0; i < 6; i++) {
        cout << i << ": " << handles[i] << endl;
    }

    HebbianNetwork *network = build_test_network(handles);

    EXPECT_TRUE(importance_equals(network->get_node_importance(handles[0]), 0.0000));
    EXPECT_TRUE(importance_equals(network->get_node_importance(handles[1]), 0.0000));
    EXPECT_TRUE(importance_equals(network->get_node_importance(handles[2]), 0.0000));
    EXPECT_TRUE(importance_equals(network->get_node_importance(handles[3]), 0.0000));
    EXPECT_TRUE(importance_equals(network->get_node_importance(handles[4]), 0.0000));
    EXPECT_TRUE(importance_equals(network->get_node_importance(handles[5]), 0.0000));

    das::HandleCount *request;
    TokenSpreader *spreader = \
        (TokenSpreader *) StimulusSpreader::factory(StimulusSpreaderType::TOKEN);

    request = new das::HandleCount();
    request->set_hebbian_network((unsigned long) network);
    (*request->mutable_handle_count())[handles[0]] = 1;
    (*request->mutable_handle_count())[handles[1]] = 1;
    (*request->mutable_handle_count())[handles[2]] = 1;
    (*request->mutable_handle_count())[handles[3]] = 1;
    (*request->mutable_handle_count())[handles[4]] = 1;
    (*request->mutable_handle_count())[handles[5]] = 1;
    (*request->mutable_handle_count())["SUM"] = 6;
    spreader->spread_stimuli(request);

    double base_importance = (double) 1 / 6;
    double expected_importance[6];
    expected_importance[0] = base_importance ;

    EXPECT_TRUE(importance_equals(network->get_node_importance(handles[0]), 0.0000));
    EXPECT_TRUE(importance_equals(network->get_node_importance(handles[1]), 0.0000));
    EXPECT_TRUE(importance_equals(network->get_node_importance(handles[2]), 0.0000));
    EXPECT_TRUE(importance_equals(network->get_node_importance(handles[3]), 0.0000));
    EXPECT_TRUE(importance_equals(network->get_node_importance(handles[4]), 0.0000));
    EXPECT_TRUE(importance_equals(network->get_node_importance(handles[5]), 0.0000));


}
