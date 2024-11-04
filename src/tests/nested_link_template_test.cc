#include <cstdlib>
#include "gtest/gtest.h"

#include "QueryNode.h"
#include "LinkTemplate.h"
#include "AtomDBSingleton.h"
#include "test_utils.h"

using namespace query_engine;
using namespace query_element;

TEST(LinkTemplate, basics) {
    
    // XXXXXX TODO: fix network info
    setenv("DAS_REDIS_HOSTNAME", "ninjato", 1);
    setenv("DAS_REDIS_PORT", "29000", 1);
    setenv("DAS_USE_REDIS_CLUSTER", "false", 1);
    setenv("DAS_MONGODB_HOSTNAME", "ninjato", 1);
    setenv("DAS_MONGODB_PORT", "28000", 1);
    setenv("DAS_MONGODB_USERNAME", "dbadmin", 1);
    setenv("DAS_MONGODB_PASSWORD", "dassecret", 1);

    string server_node_id = "SERVER";
    QueryNodeServer server_node(server_node_id);

    AtomDBSingleton::init();
    string expression = "Expression";
    string symbol = "Symbol";

    Variable v1("v1");
    Variable v2("v2");
    Variable v3("v3");
    Node similarity(symbol, "Similarity");
    Node odd_similarity(symbol, "OddSimilarity");

    LinkTemplate<3> inner_template("Expression", {&similarity, &v1, &v2});
    LinkTemplate<2> outter_template("Expression", {&odd_similarity, &inner_template});

    outter_template.subsequent_id = server_node_id;
    outter_template.setup_buffers();
    //outter_template.fetch_links();
    Utils::sleep(1000);

    DASQueryAnswer *query_answer;
    unsigned int count;
    while ((query_answer = server_node.pop_query_answer()) != NULL) {
        EXPECT_TRUE(double_equals(query_answer->importance, 0.0));
        count++;
    }
    EXPECT_EQ(count, 6);
}
