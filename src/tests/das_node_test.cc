#include <cstdlib>
#include "gtest/gtest.h"

#include "DASNode.h"
#include "AtomDBSingleton.h"
#include "test_utils.h"

using namespace query_engine;

void check_query(
    vector<string> &query,
    unsigned int expected_count) {

    cout << "XXXXXXXXXXXXXXXX DASNode.queries CHECK BEGIN" << endl;
    string das_id = "localhost:30700";
    string requestor_id = "localhost:30701";
    DASNode das(das_id);
    Utils::sleep(1000);
    DASNode requestor(requestor_id, das_id);
    Utils::sleep(1000);
    QueryAnswer *query_answer;
    RemoteIterator *response = requestor.pattern_matcher_query(query);
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
    cout << "XXXXXXXXXXXXXXXX DASNode.queries CHECK END" << endl;
}

TEST(DASNode, queries) {

    cout << "XXXXXXXXXXXXXXXX DASNode.queries BEGIN" << endl;

    setenv("DAS_REDIS_HOSTNAME", "ninjato", 1);
    setenv("DAS_REDIS_PORT", "29000", 1);
    setenv("DAS_USE_REDIS_CLUSTER", "false", 1);
    setenv("DAS_MONGODB_HOSTNAME", "ninjato", 1);
    setenv("DAS_MONGODB_PORT", "28000", 1);
    setenv("DAS_MONGODB_USERNAME", "dbadmin", 1);
    setenv("DAS_MONGODB_PASSWORD", "dassecret", 1);
    AtomDBSingleton::init();

    /*
    string das_id = "localhost:30700";
    string requestor_id = "localhost:30701";
    DASNode das(das_id);
    Utils::sleep(1000);
    DASNode requestor(requestor_id, das_id);
    Utils::sleep(1000);

    unsigned int count;
    QueryAnswer *query_answer;

    q1 = {
        "atom_type": "link",
        "type": "Expression",
        "targets": [
            {"atom_type": "node", "type": "Symbol", "name": "Similarity"},
            {"atom_type": "variable", "name": "v1"},
            {"atom_type": "variable", "name": "v2"},
        ]
    }
    */
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
            "VARIABLE", "v2" 
    };

    /*
    RemoteIterator *response = requestor.pattern_matcher_query(q1);
    count = 0;
    cout << "XXXXXXXXXXXXXXXX DASNode.queries 1" << endl;
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
    EXPECT_EQ(count, 14);

    cout << "XXXXXXXXXXXXXXXX DASNode.queries 2" << endl;
    */

    check_query(q1, 14);
    check_query(q2, 3);

    cout << "XXXXXXXXXXXXXXXX DASNode.queries BEGIN" << endl;
}
