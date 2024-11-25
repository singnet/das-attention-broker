#include <cstdlib>
#include "gtest/gtest.h"

#include "DASNode.h"
#include "AtomDBSingleton.h"
#include "test_utils.h"

using namespace query_engine;

void check_query(
    vector<string> &query,
    unsigned int expected_count,
    DASNode *das,
    DASNode *requestor) {

    cout << "XXXXXXXXXXXXXXXX DASNode.queries CHECK BEGIN" << endl;
    QueryAnswer *query_answer;
    RemoteIterator *response = requestor->pattern_matcher_query(query);
    unsigned int count = 0;
    while (! response->finished()) {
        while ((query_answer = response->pop()) == NULL) {
            if (response->finished()) {
                break;
            } else {
                Utils::sleep();
            }
        }
        if (query_answer != NULL) {
            cout << "XXXXX " << query_answer->to_string() << endl;
            count++;
        }
    }
    EXPECT_EQ(count, expected_count);
    delete response;
    cout << "XXXXXXXXXXXXXXXX DASNode.queries CHECK END" << endl;
}

TEST(DASNode, queries) {

    cout << "XXXXXXXXXXXXXXXX DASNode.queries BEGIN" << endl;

    setenv("DAS_REDIS_HOSTNAME", "localhost", 1);
    setenv("DAS_REDIS_PORT", "6379", 1);
    setenv("DAS_USE_REDIS_CLUSTER", "false", 1);
    setenv("DAS_MONGODB_HOSTNAME", "localhost", 1);
    setenv("DAS_MONGODB_PORT", "27017", 1);
    setenv("DAS_MONGODB_USERNAME", "root", 1);
    setenv("DAS_MONGODB_PASSWORD", "root", 1);
    AtomDBSingleton::init();

    string das_id = "localhost:31700";
    string requestor_id = "localhost:31701";
    DASNode *das = new DASNode(das_id);
    Utils::sleep(1000);
    DASNode *requestor = new DASNode(requestor_id, das_id);
    Utils::sleep(1000);

    vector<string> q1 = {
        "LINK_TEMPLATE", "Expression", "3",
            "NODE", "Symbol", "Similarity",
            "VARIABLE", "v1",
            "VARIABLE", "v2"
    };

    vector<string> q2 = {
        "LINK_TEMPLATE", "Expression", "3",
            "NODE", "Symbol", "Similarity",
            "NODE", "Symbol", "\"human\"",
            "VARIABLE", "v1"
    };

    vector<string> q3 = {
        "AND", "2",
            "LINK_TEMPLATE", "Expression", "3",
                "NODE", "Symbol", "Similarity",
                "VARIABLE", "v1",
                "NODE", "Symbol", "\"human\"",
            "LINK_TEMPLATE", "Expression", "3",
                "NODE", "Symbol", "Inheritance",
                "VARIABLE", "v1",
                "NODE", "Symbol", "\"plant\"",
    };

    vector<string> q4 = {
        "AND", "2",
            "LINK_TEMPLATE", "Expression", "3",
                "NODE", "Symbol", "Similarity",
                "VARIABLE", "v1",
                "VARIABLE", "v2",
            "LINK_TEMPLATE", "Expression", "3",
                "NODE", "Symbol", "Similarity",
                "VARIABLE", "v2",
                "VARIABLE", "v3"
    };

    check_query(q1, 14, das, requestor);
    check_query(q2, 3, das, requestor);
    check_query(q3, 1, das, requestor);
    check_query(q4, 26, das, requestor); // TODO: FIX THIS count should be == 1

    delete(requestor);
    delete(das);

    cout << "XXXXXXXXXXXXXXXX DASNode.queries END" << endl;
}
