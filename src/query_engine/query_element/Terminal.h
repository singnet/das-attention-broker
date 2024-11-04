#ifndef _QUERY_ELEMENT_TERMINAL_H
#define _QUERY_ELEMENT_TERMINAL_H

#include <string>
#include <string.h>
#include <array>
#include "QueryElement.h"
#include "AtomDB.h"
#include "expression_hasher.h"

using namespace std;
using namespace query_engine;

namespace query_element {

/**
 *
 */
class Terminal : public QueryElement {
public:
    Terminal() {
        this->handle = shared_ptr<char>{};
        this->is_variable = false;
    }
    ~Terminal() {
    };
    void setup_buffers() {}
    void graceful_shutdown() {}
    bool is_terminal() {return true; }
    virtual string to_string() {
        return "TERMINAL";
    }
    bool is_variable;
    shared_ptr<char> handle;
    string name;
};

class Node : public Terminal {
public:
    Node(const string &type, const string &name) {
        this->type = type;
        this->name = name;
        this->handle = shared_ptr<char>(terminal_hash((char *) type.c_str(), (char *) name.c_str()));
    }
    string to_string() {
        return "<" + this->type + ", " + this->name + ", " + string(this->handle.get()) + ">";
    }
    string type;
};

template <unsigned int ARITY>
class Link : public Terminal {
public:
    Link(const string &type, const array<string, ARITY> &targets) {
        this->name = "";
        this->type = type;
        this->targets = targets;
        this->arity = ARITY;
        char *handle_keys[ARITY + 1];
        bool wildcard_flag = (type == AtomDB::WILDCARD);
        handle_keys[0] = (wildcard_flag ? 
            (char *) AtomDB::WILDCARD.c_str() : 
            (char *) named_type_hash(type.c_str()));
        for (unsigned int i = 1; i < (ARITY + 1); i++) {
            handle_keys[i] = targets[i].c_str;
        }
        this->handle = shared_ptr<char>(composite_hash(handle_keys, ARITY + 1));
        if (! wildcard_flag) {
            free(handle_keys[0]);
        }
    }
    string to_string() {
        string answer = "(" + this->type + ", [";
        for (unsigned int i = 0; i < this->arity; i++) {
            answer += this->targets[i];
            if (i != (this->arity - 1)) {
                answer += ", ";
            }
        }
        answer += "], " + string(this->handle.get()) + ")";
        return answer;
    }
    string type;
    unsigned int arity = ARITY;
    array<string, ARITY> targets;
};

class Variable : public Terminal {
public:
    Variable(const string &name) : Terminal() {
        this->name = name;
        this->handle = shared_ptr<char>(strdup((char *) AtomDB::WILDCARD.c_str()));
        this->is_variable = true;
    }
    string to_string() {
        return "$(" + this->name + ")";
    }
};

} // namespace query_element

#endif // _QUERY_ELEMENT_TERMINAL_H
