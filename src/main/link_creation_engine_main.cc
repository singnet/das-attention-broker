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

std::vector<std::string> split(string s, string delimiter) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s);

    return tokens;
}

double compute_sim1(const vector<string> tokens1, const vector<string> tokens2) {

    unsigned int count = 0;

    for (auto token1: tokens1) {
        for (auto token2: tokens2) {
            if (token1 == token2) {
                count++;
                break;
            }
        }
    }

    return ((1.0) * count) / tokens1.size();
}

void build_sim1_link(const string str1, const string str2, double threshold) {

    vector<string> tokens1 = split(str1.substr(1, str1.size() - 2), " ");
    vector<string> tokens2 = split(str2.substr(1, str2.size() - 2), " ");

    double v1 = compute_sim1(tokens1, tokens2);
    double v2 = 0.0;

    if (v1 >= threshold) {
        cout << str1 << " " << v1 << endl;
        cout << str2 << " " << v2 << endl;
        cout << endl;
    }
}

string handle_to_atom(const char *handle) {

    shared_ptr<AtomDB> db = AtomDBSingleton::get_instance();
    shared_ptr<atomdb_api_types::AtomDocument> document = db->get_atom_document(handle);
    shared_ptr<atomdb_api_types::HandleList> targets = db->query_for_targets((char *) handle);
    string answer;

    if (targets != NULL) {
        // is link
        answer += "<";
        answer += document->get("named_type");
        answer += ": [";
        for (unsigned int i = 0; i < targets->size(); i++) {
            answer += handle_to_atom(targets->get_handle(i));
            if (i < (targets->size() - 1)) {
                answer += ", ";
            }
        }
        answer += ">";
    } else {
        // is node
        answer += "(";
        answer += document->get("named_type");
        answer += ": ";
        answer += document->get("name");
        answer += ")";
    }

    return answer;
}

void populate_sentences_1(
    const string &context,
    const string &link_type_tag,
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

    // (Contains (Sentence "aef cbe dfb fbe eca eff bad") (Word "eff"))

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
    shared_ptr<atomdb_api_types::AtomDocument> sentence_document1;
    shared_ptr<atomdb_api_types::AtomDocument> sentence_document2;
    shared_ptr<atomdb_api_types::AtomDocument> sentence_symbol_document1;
    shared_ptr<atomdb_api_types::AtomDocument> sentence_symbol_document2;
    while (! response->finished()) {
        if ((query_answer = response->pop()) == NULL) {
            Utils::sleep();
        } else {
            if (! strcmp(query_answer->assignment.get(sentence1.c_str()), query_answer->assignment.get(sentence2.c_str()))) {
                continue;
            }
            //cout << query_answer->to_string() << endl;
            //cout << handle_to_atom(query_answer->handles[0]) << endl;
            //cout << handle_to_atom(query_answer->handles[1]) << endl;
            sentence_document1 = db->get_atom_document(query_answer->assignment.get(sentence1.c_str()));
            sentence_document2 = db->get_atom_document(query_answer->assignment.get(sentence2.c_str()));
            sentence_symbol_document1 = db->get_atom_document(sentence_document1->get("targets", 1));
            sentence_symbol_document2 = db->get_atom_document(sentence_document2->get("targets", 1));
            string s1 = string(sentence_symbol_document1->get("name"));
            string s2 = string(sentence_symbol_document2->get("name"));
            build_sim1_link(s1, s2, 0.0);

            if (++count == MAX_QUERY_ANSWERS) {
                break;
            }
        }
    }
    if (count == 0) {
        cout << "No match for query" << endl;
    }

    delete response;
}

void populate_sentences_2(
    const string &context,
    const string &link_type_tag,
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
            sentences.push_back(string(word_document->get("name")));
            if (++count == MAX_QUERY_ANSWERS) {
                break;
            }
        }
    }
    if (count == 0) {
        cout << "No match for query" << endl;
        exit(0);
    }

    for (unsigned int i = 0; i < sentences.size() - 1;  i++) {
        for (unsigned int j = i + 1; j < sentences.size(); j++) {
            if (link_type_tag == "SIM1") {
                build_sim1_link(sentences[i], sentences[j], 0.2);
            } else if (link_type_tag == "SIM2") {
                Utils::error("SIM2 not implemented yet.");
            } else {
                Utils::error("Invalid link_type_tag: " + link_type_tag + " (it should be SIM1 or SIM2)");
            }
        }
    }

    delete response;
}

int main(int argc, char* argv[]) {

    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <context> <link type tag> <word tag>" << endl;
        exit(1);
    }
    signal(SIGINT, &ctrl_c_handler);
    string context = argv[1];
    string link_type_tag = argv[2];
    string word_tag = argv[3];

    populate_sentences_1(context, link_type_tag, word_tag);
    return 0;
}
