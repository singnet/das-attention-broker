#include <cstdlib>
#include "gtest/gtest.h"

#include "RemoteSink.h"
#include "Source.h"
#include "RemoteIterator.h"
#include "AtomDBSingleton.h"
#include "test_utils.h"

using namespace query_engine;
using namespace query_element;
using namespace query_node;

class TestSource : public Source {
    public:
    TestSource(const string &id) {
        this->id = id;
    }
    void add(DASQueryAnswer *qa) {
        this->output_buffer->add_query_answer(qa);
        Utils::sleep(1000);
    }
    void finished() {
        this->output_buffer->query_answers_finished();
        Utils::sleep(1000);
    }
};

TEST(RemoteSinkIterator, basics) {
    string producer_id = "localhost:30700";
    string consumer_id = "localhost:30701";

    string input_element_id = "test_source";
    TestSource input(input_element_id);
    RemoteIterator consumer(consumer_id);
    RemoteSink producer(&input, producer_id, consumer_id);
    producer.setup_buffers();
    Utils::sleep(1000);

    EXPECT_FALSE(consumer.finished());

    DASQueryAnswer *qa;
    DASQueryAnswer qa0("h0", 0.0);
    DASQueryAnswer qa1("h1", 0.1);
    DASQueryAnswer qa2("h2", 0.2);

    input.add(&qa0);
    input.add(&qa1);

    EXPECT_FALSE(consumer.finished());
    EXPECT_FALSE((qa = consumer.pop()) == NULL);
    EXPECT_TRUE(strcmp(qa->handles[0], "h0") == 0);
    EXPECT_TRUE(double_equals(qa->importance, 0.0));

    EXPECT_FALSE(consumer.finished());
    EXPECT_FALSE((qa = consumer.pop()) == NULL);
    EXPECT_TRUE(strcmp(qa->handles[0], "h1") == 0);
    EXPECT_TRUE(double_equals(qa->importance, 0.1));

    EXPECT_TRUE((qa = consumer.pop()) == NULL);
    EXPECT_FALSE(consumer.finished());

    input.add(&qa2);
    input.finished();
    EXPECT_FALSE(consumer.finished());

    EXPECT_FALSE(consumer.finished());
    EXPECT_FALSE((qa = consumer.pop()) == NULL);
    EXPECT_TRUE(strcmp(qa->handles[0], "h2") == 0);
    EXPECT_TRUE(double_equals(qa->importance, 0.2));
    EXPECT_TRUE(consumer.finished());
}

/*
TEST(Iterator, link_template_integration) {

    // XXXXXX TODO: fix network info
    setenv("DAS_REDIS_HOSTNAME", "ninjato", 1);
    setenv("DAS_REDIS_PORT", "29000", 1);
    setenv("DAS_USE_REDIS_CLUSTER", "false", 1);
    setenv("DAS_MONGODB_HOSTNAME", "ninjato", 1);
    setenv("DAS_MONGODB_PORT", "28000", 1);
    setenv("DAS_MONGODB_USERNAME", "dbadmin", 1);
    setenv("DAS_MONGODB_PASSWORD", "dassecret", 1);
    cout << "XXXXXXXXXX link_template_integration() 1" << endl;

    AtomDBSingleton::init();
    string expression = "Expression";
    string symbol = "Symbol";
    cout << "XXXXXXXXXX link_template_integration() 2" << endl;

    Variable v1("v1");
    Variable v2("v2");
    Variable v3("v3");
    Node similarity(symbol, "Similarity");
    Node human(symbol, "\"human\"");
    cout << "XXXXXXXXXX link_template_integration() 3" << endl;

    LinkTemplate<3> link_template("Expression", {&similarity, &human, &v1});
    cout << "XXXXXXXXXX link_template_integration() 3.1" << endl;
    Iterator query_answer_iterator(&link_template);
    cout << "XXXXXXXXXX link_template_integration() 3.2" << endl;
    link_template.fetch_links();
    cout << "XXXXXXXXXX link_template_integration() 4" << endl;

    string monkey_handle = string(terminal_hash((char *) symbol.c_str(), (char *) "\"monkey\""));
    string chimp_handle = string(terminal_hash((char *) symbol.c_str(), (char *) "\"chimp\""));
    string ent_handle = string(terminal_hash((char *) symbol.c_str(), (char *) "\"ent\""));
    bool monkey_flag = false;
    bool chimp_flag = false;
    bool ent_flag = false;
    DASQueryAnswer *query_answer;
    cout << "XXXXXXXXXX link_template_integration() 5" << endl;
    while (! query_answer_iterator.finished()) {
        query_answer = query_answer_iterator.pop();
        if (query_answer != NULL) {
            string var = string(query_answer->assignment.get("v1"));
            EXPECT_TRUE(double_equals(query_answer->importance, 0.0));
            if (var == monkey_handle) {
                // TODO: perform extra checks
                monkey_flag = true;
            } else if (var == chimp_handle) {
                // TODO: perform extra checks
                chimp_flag = true;
            } else if (var == ent_handle) {
                // TODO: perform extra checks
                ent_flag = true;
            } else {
                FAIL();
            }
        }
    }
    cout << "XXXXXXXXXX link_template_integration() 6" << endl;
    EXPECT_TRUE(monkey_flag);
    EXPECT_TRUE(chimp_flag);
    EXPECT_TRUE(ent_flag);
    cout << "XXXXXXXXXX link_template_integration() 7" << endl;
    cout << "XXXXXXXXXXX FINISHED XXXXXXXXXXXXX" << endl;
}
*/
