#include <iostream>
#include <string>

#include <signal.h>

#include "DASNode.h"
#include "RemoteIterator.h"
#include "QueryAnswer.h"
#include "AtomDBSingleton.h"
#include "AtomDB.h"
#include "Utils.h"

#define MAX_QUERY_ANSWERS ((unsigned int) 100)

using namespace std;

void ctrl_c_handler(int) {
    std::cout << "Stopping link creation engine server..." << std::endl;
    std::cout << "Done." << std::endl;
    exit(0);
}

void run(
    const string &context,
    const string &word_tag) {

    string server_id = "localhost:31700";
    string client_id = "localhost:31701";

    AtomDBSingleton::init();
    shared_ptr<AtomDB> db = AtomDBSingleton::get_instance();

    string and_operator = "AND";
    string link_template = "LINK_TEMPLATE";
    string link = "LINK";
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
        link_template, expression, "3", 
            node, symbol, contains, 
            variable, sentence1, 
            link, expression, "2", 
                node, symbol, word, 
                node, symbol, "\"" + word_tag + "\""
    };

    DASNode client(client_id, server_id);
    QueryAnswer *query_answer;
    unsigned int count = 0;
    RemoteIterator *response = client.pattern_matcher_query(query_word);
    shared_ptr<atomdb_api_types::AtomDocument> sentence_document;
    shared_ptr<atomdb_api_types::AtomDocument> word_document;
    vector<string> sentences;
    while (! response->finished()) {
        if ((query_answer = response->pop()) == NULL) {
            Utils::sleep();
        } else {
            //cout << query_answer->to_string() << endl;
            sentence_document = db->get_atom_document(query_answer->assignment.get(sentence1.c_str()));
            word_document = db->get_atom_document(sentence_document->get("targets", 1));
            cout << string(word_document->get("name"));
            if (++count == MAX_QUERY_ANSWERS) {
                break;
            }
        }
    }
    if (count == 0) {
        cout << "No match for query" << endl;
        exit(0);
    }

    delete response;
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <context> <word tag>" << endl;
        exit(1);
    }
    signal(SIGINT, &ctrl_c_handler);
    string context = argv[1];
    string word_tag = argv[3];

    run(context, word_tag);
    return 0;
}
