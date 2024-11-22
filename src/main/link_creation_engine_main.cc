#include <iostream>
#include <string>

#include <signal.h>

#include "DASNode.h"
#include "RemoteIterator.h"
#include "QueryAnswer.h"
#include "AtomDBSingleton.h"
#include "AtomDB.h"
#include "Utils.h"

#define MAX_QUERY_ANSWERS ((unsigned int) 1000)

using namespace std;

void ctrl_c_handler(int) {
    std::cout << "Stopping link creation engine server..." << std::endl;
    std::cout << "Done." << std::endl;
    exit(0);
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <context> <link type tag>" << endl;
        exit(1);
    }
    signal(SIGINT, &ctrl_c_handler);

    string server_id = "localhost:31700";
    string client_id = "localhost:31701";

    string and_operator = "AND";
    string link_template = "LINK_TEMPLATE";
    string node = "NODE";
    string variable = "VARIABLE";
    string expression = "Expression";
    string symbol = "Symbol";
    string sentence = "Sentence";
    string word = "Word";
    string contains = "Contains";
    string sentence1 = "sentence1";
    string sentence2 = "sentence2";
    string word1 = "word1";
    string word2 = "word2";

    vector<string> query_word = {
        and_operator, "2",
            link_template, expression, "3",
                node, symbol, contains,
                variable, sentence1,
                variable, word1,
            link_template, expression, "3",
                node, symbol, contains,
                variable, sentence2,
                variable, word1
    };

    DASNode client(client_id, server_id);
    QueryAnswer *query_answer;
    unsigned int count = 0;
    RemoteIterator *response = client.pattern_matcher_query(query_word);
    while (! response->finished()) {
        if ((query_answer = response->pop()) == NULL) {
            Utils::sleep();
        } else {
            cout << query_answer->to_string() << endl;
            string handle = string(query_answer->assignment.get(word1.c_str()));
            cout << "Word handle: " << handle;
            if (++count == MAX_QUERY_ANSWERS) {
                break;
            }
        }
    }
    if (count == 0) {
        cout << "No match for query" << endl;
    }

    delete response;
    return 0;
}
