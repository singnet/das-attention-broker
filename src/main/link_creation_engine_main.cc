#include <iostream>
#include <string>
#include <stack>

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

string highlight(const string &s, const set<string> &highlighted) {
    vector<string> tokens = split(s.substr(1, s.size() - 2), " ");
    string answer = "";
    for (unsigned int i = 0; i < tokens.size(); i++) {
        if (highlighted.find(tokens[i]) != highlighted.end()) {
            //"\033[31;1;4mHello\033[0m"
            answer += "\033[1;4m" + tokens[i] + "\033[0m";
        } else {
            answer += tokens[i];
        }
        if (i != (tokens.size() - 1)) {
            answer += " ";
        }
    }
    return answer;
}

void build_sim1_link(const string str1, const string str2, double threshold, stack<string> &output, const set<string> &highlighted) {

    vector<string> tokens1 = split(str1.substr(1, str1.size() - 2), " ");
    vector<string> tokens2 = split(str2.substr(1, str2.size() - 2), " ");

    double v1 = compute_sim1(tokens1, tokens2);
    double v2 = 0.0;

    if (v1 >= threshold) {
        output.push(highlight(str1, highlighted) + " " + std::to_string(v1));
        output.push(highlight(str2, highlighted) + " " + std::to_string(v2));
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

void run(
    const string &context,
    const string &link_type_tag,
    const set<string> highlighted) {

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
    RemoteIterator *response = client.pattern_matcher_query(query_word, context);
    shared_ptr<atomdb_api_types::AtomDocument> sentence_document1;
    shared_ptr<atomdb_api_types::AtomDocument> sentence_document2;
    shared_ptr<atomdb_api_types::AtomDocument> sentence_symbol_document1;
    shared_ptr<atomdb_api_types::AtomDocument> sentence_symbol_document2;
    stack<string> output;
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
            build_sim1_link(s1, s2, 0.0, output, highlighted);

            if (++count == MAX_QUERY_ANSWERS) {
                break;
            }
        }
    }
    if (count == 0) {
        cout << "No match for query" << endl;
    } else {
        while (! output.empty()) {
            cout << output.top() << endl;
            output.pop();
            cout << output.top() << endl;
            output.pop();
            cout << endl;
        }
    }

    delete response;
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <context> <link type tag> <word>*" << endl;
        exit(1);
    }
    signal(SIGINT, &ctrl_c_handler);
    string context = argv[1];
    string link_type_tag = argv[2];

    set<string> highlighted;
    for (int i = 3; i < argc; i++) {
        highlighted.insert(string(argv[i]));
    }

    run(context, link_type_tag, highlighted);
    return 0;
}
