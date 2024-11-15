#include <cstdlib>
#include "gtest/gtest.h"

#include "QueryNode.h"
#include "AtomDB.h"
#include "LinkTemplate.h"
#include "AtomDBSingleton.h"
#include "test_utils.h"

using namespace query_engine;
using namespace query_element;

TEST(LinkTemplate, basics) {
    
    cout << "XXXXX basics BEGIN" << endl;

    setenv("DAS_REDIS_HOSTNAME", "localhost", 1);
    setenv("DAS_REDIS_PORT", "29000", 1);
    setenv("DAS_USE_REDIS_CLUSTER", "false", 1);
    setenv("DAS_MONGODB_HOSTNAME", "localhost", 1);
    setenv("DAS_MONGODB_PORT", "28000", 1);
    setenv("DAS_MONGODB_USERNAME", "dbadmin", 1);
    setenv("DAS_MONGODB_PASSWORD", "dassecret", 1);

    cout << "XXXXX basics 1" << endl;
    string server_node_id = "SERVER";
    QueryNodeServer server_node(server_node_id);

    cout << "XXXXX basics 2" << endl;
    AtomDBSingleton::init();
    string expression = "Expression";
    string symbol = "Symbol";

    cout << "XXXXX basics 3" << endl;
    Variable v1("v1");
    Variable v2("v2");
    Variable v3("v3");
    Node similarity(symbol, "Similarity");
    Node odd_link(symbol, "OddLink");

    cout << "XXXXX basics 4" << endl;
    LinkTemplate<3> inner_template("Expression", {&similarity, &v1, &v2});
    LinkTemplate<2> outter_template("Expression", {&odd_link, &inner_template});

    cout << "XXXXX basics 5" << endl;
    outter_template.subsequent_id = server_node_id;
    outter_template.setup_buffers();
    Utils::sleep(5000);

    cout << "XXXXX basics 6" << endl;
    QueryAnswer *query_answer;
    unsigned int count = 0;
    while ((query_answer = server_node.pop_query_answer()) != NULL) {
        EXPECT_TRUE(double_equals(query_answer->importance, 0.0));
        count++;
    }
    EXPECT_EQ(count, 8);
    cout << "XXXXX basics END" << endl;
}

TEST(LinkTemplate, nested_variables) {
    
    cout << "XXXXX nested_variables BEGIN" << endl;
    string server_node_id = "SERVER";
    QueryNodeServer server_node(server_node_id);
    cout << "XXXXX nested_variables 1" << endl;

    string expression = "Expression";
    string symbol = "Symbol";

    cout << "XXXXX nested_variables 2" << endl;
    Variable v1("v1");
    Variable v2("v2");
    Variable v3("v3");
    Node similarity(symbol, "Similarity");
    Node odd_link(symbol, "OddLink");
    Node human(symbol, "\"human\"");
    cout << "XXXXX nested_variables 3" << endl;

    LinkTemplate<3> inner_template("Expression", {&similarity, &v1, &v2});
    LinkTemplate<2> outter_template("Expression", {&odd_link, &inner_template});
    LinkTemplate<3> human_template("Expression", {&similarity, &v1, &human});
    And<2> and_operator({&human_template, &outter_template});
    cout << "XXXXX nested_variables 4" << endl;

    and_operator.subsequent_id = server_node_id;
    and_operator.setup_buffers();
    Utils::sleep(5000);
    cout << "XXXXX nested_variables 5" << endl;

    QueryAnswer *query_answer;
    shared_ptr<atomdb_api_types::AtomDocument> document;
    unsigned int count = 0;
    cout << "XXXXX nested_variables 6" << endl;
    while ((query_answer = server_node.pop_query_answer()) != NULL) {
        EXPECT_TRUE(double_equals(query_answer->importance, 0.0));
        Utils::sleep();
        count++;
    }
    cout << "XXXXX nested_variables 7" << endl;
    EXPECT_EQ(count, 1);
    cout << "XXXXX nested_variables END" << endl;
}
