#include <array>
#include "DASNode.h"
#include "LinkTemplate.h"
#include "Terminal.h"
#include "RemoteSink.h"

using namespace query_engine;

string DASNode::PATTERN_MATCHING_QUERY = "pattern_matching_query";

// -------------------------------------------------------------------------------------------------
// Constructors and destructors

DASNode::DASNode(const string &node_id) : StarNode(node_id) {
    cout << "XXXXXXXX COSNTRUCTOR DASNode server BEGIN " << std::to_string((unsigned long) this) << endl;
    initialize();
    cout << "XXXXXXXX COSNTRUCTOR DASNode server END " << std::to_string((unsigned long) this) << endl;
    // SERVER
}

DASNode::DASNode(const string &node_id, const string &server_id) : StarNode(node_id, server_id) {
    cout << "XXXXXXXX COSNTRUCTOR DASNode client BEGIN " << std::to_string((unsigned long) this) << endl;
    initialize();
    cout << "XXXXXXXX COSNTRUCTOR DASNode client END " << std::to_string((unsigned long) this) << endl;
    // CLIENT
}

DASNode::~DASNode() {
    cout << "XXXXXXXX DESTRUCTOR DASNode BEGIN " << std::to_string((unsigned long) this) << endl;
    cout << "XXXXXXXX DESTRUCTOR DASNode END " << std::to_string((unsigned long) this) << endl;
}

void DASNode::initialize() {
    this->first_query_port = 60000;
    this->last_query_port = 61999;
    string id = this->node_id();
    this->local_host = id.substr(0, id.find(":"));
    if (this->is_server) {
        this->next_query_port = this->first_query_port; 
    } else {
        this->next_query_port = (this->first_query_port + this->last_query_port) / 2; 
    }
}

// -------------------------------------------------------------------------------------------------
// Public client API

RemoteIterator *DASNode::pattern_matcher_query(const vector<string> &tokens) {
    if (this->is_server) {
        Utils::error("pattern_matcher_query() is not available in DASNode server.");
    }
    // TODO XXX change this when requestor is set in basic Message
    string query_id = next_query_id();
    vector<string> args = {query_id};
    args.insert(args.end(), tokens.begin(), tokens.end());
    cout << "XXXXXXXX pattern_matcher_query() sendind message to " << this->server_id << endl;
    send(PATTERN_MATCHING_QUERY, args, this->server_id);
    return new RemoteIterator(query_id);
}

// -------------------------------------------------------------------------------------------------
// Public generic methods

string DASNode::next_query_id() {
    unsigned int port = this->next_query_port++;
    unsigned int limit;
    if (this->is_server) {
        limit = ((this->first_query_port + this->last_query_port) / 2) - 1; 
        if (this->next_query_port > limit) {
            this->next_query_port = this->first_query_port;
        }
    } else {
        limit = this->last_query_port; 
        if (this->next_query_port > limit) {
            this->next_query_port = (this->first_query_port + this->last_query_port) / 2; 
        }
    }
    return this->local_host + ":" + std::to_string(port);
}

// -------------------------------------------------------------------------------------------------
// Messages

shared_ptr<Message> DASNode::message_factory(string &command, vector<string> &args) {
    std::shared_ptr<Message> message = AtomSpaceNode::message_factory(command, args);
    if (message) {
        return message;
    }
    if (command == DASNode::PATTERN_MATCHING_QUERY) {
        return std::shared_ptr<Message>(new PatternMatchingQuery(command, args));
    }
    return std::shared_ptr<Message>{};
}

QueryElement *PatternMatchingQuery::build_link_template(
    vector<string> &tokens, 
    unsigned int cursor,
    stack<QueryElement *> &element_stack) {

    unsigned int arity = std::stoi(tokens[cursor + 2]);
    if (element_stack.size() < arity) {
        Utils::error("PatternMatchingQuery message: parse error in tokens - too few arguments for LINK_TEMPLATE");
    }
    switch (arity) {
        // TODO: consider replacing each "case" below by a pre-processor macro call
        case 1: {
            array<QueryElement *, 1> targets;
            for (unsigned int i = 0; i < 1; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new LinkTemplate<1>(tokens[cursor + 1], targets);
        }
        case 2: {
            array<QueryElement *, 2> targets;
            for (unsigned int i = 0; i < 2; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new LinkTemplate<2>(tokens[cursor + 1], targets);
        }
        case 3: {
            array<QueryElement *, 3> targets;
            for (unsigned int i = 0; i < 3; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new LinkTemplate<3>(tokens[cursor + 1], targets);
        }
        case 4: {
            array<QueryElement *, 4> targets;
            for (unsigned int i = 0; i < 4; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new LinkTemplate<4>(tokens[cursor + 1], targets);
        }
        case 5: {
            array<QueryElement *, 5> targets;
            for (unsigned int i = 0; i < 5; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new LinkTemplate<5>(tokens[cursor + 1], targets);
        }
        case 6: {
            array<QueryElement *, 6> targets;
            for (unsigned int i = 0; i < 6; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new LinkTemplate<6>(tokens[cursor + 1], targets);
        }
        case 7: {
            array<QueryElement *, 7> targets;
            for (unsigned int i = 0; i < 7; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new LinkTemplate<7>(tokens[cursor + 1], targets);
        }
        case 8: {
            array<QueryElement *, 8> targets;
            for (unsigned int i = 0; i < 8; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new LinkTemplate<8>(tokens[cursor + 1], targets);
        }
        case 9: {
            array<QueryElement *, 9> targets;
            for (unsigned int i = 0; i < 9; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new LinkTemplate<9>(tokens[cursor + 1], targets);
        }
        case 10: {
            array<QueryElement *, 10> targets;
            for (unsigned int i = 0; i < 10; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new LinkTemplate<10>(tokens[cursor + 1], targets);
        }
        default: {
            Utils::error("PatternMatchingQuery message: max supported arity for LINK_TEMPLATE: 10");
        }
    }
    return NULL; // Just to avoid warnings. This is not actually reachable.
}

QueryElement *PatternMatchingQuery::build_link(
    vector<string> &tokens, 
    unsigned int cursor,
    stack<QueryElement *> &element_stack) {

    unsigned int arity = std::stoi(tokens[cursor + 2]);
    if (element_stack.size() < arity) {
        Utils::error("PatternMatchingQuery message: parse error in tokens - too few arguments for LINK");
    }
    switch (arity) {
        // TODO: consider replacing each "case" below by a pre-processor macro call
        case 1: {
            array<QueryElement *, 1> targets;
            for (unsigned int i = 0; i < 1; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new Link<1>(tokens[cursor + 1], targets);
        }
        case 2: {
            array<QueryElement *, 2> targets;
            for (unsigned int i = 0; i < 2; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new Link<2>(tokens[cursor + 1], targets);
        }
        case 3: {
            array<QueryElement *, 3> targets;
            for (unsigned int i = 0; i < 3; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new Link<3>(tokens[cursor + 1], targets);
        }
        case 4: {
            array<QueryElement *, 4> targets;
            for (unsigned int i = 0; i < 4; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new Link<4>(tokens[cursor + 1], targets);
        }
        case 5: {
            array<QueryElement *, 5> targets;
            for (unsigned int i = 0; i < 5; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new Link<5>(tokens[cursor + 1], targets);
        }
        case 6: {
            array<QueryElement *, 6> targets;
            for (unsigned int i = 0; i < 6; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new Link<6>(tokens[cursor + 1], targets);
        }
        case 7: {
            array<QueryElement *, 7> targets;
            for (unsigned int i = 0; i < 7; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new Link<7>(tokens[cursor + 1], targets);
        }
        case 8: {
            array<QueryElement *, 8> targets;
            for (unsigned int i = 0; i < 8; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new Link<8>(tokens[cursor + 1], targets);
        }
        case 9: {
            array<QueryElement *, 9> targets;
            for (unsigned int i = 0; i < 9; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new Link<9>(tokens[cursor + 1], targets);
        }
        case 10: {
            array<QueryElement *, 10> targets;
            for (unsigned int i = 0; i < 10; i++) {
                targets[i] = element_stack.top();
                element_stack.pop();
            }
            return new Link<10>(tokens[cursor + 1], targets);
        }
        default: {
            Utils::error("PatternMatchingQuery message: max supported arity for LINK: 10");
        }
    }
    return NULL; // Just to avoid warnings. This is not actually reachable.
}

PatternMatchingQuery::PatternMatchingQuery(string command, vector<string> &tokens) {

    cout << "XXXXXXXX COSNTRUCTOR PatternMatchingQuery BEGIN " << std::to_string((unsigned long) this) << endl;

    stack<unsigned int> execution_stack;
    stack<QueryElement *> element_stack;
    this->requestor_id = tokens[0];
    unsigned int cursor = 1; // TODO XXX: change this when requestor is set in basic Message
    unsigned int tokens_count = tokens.size();

    while (cursor < tokens_count) {
        execution_stack.push(cursor);
        if (tokens[cursor] == "VARIABLE") {
            cursor += 2;
        } else {
            cursor += 3;
        }
    }
    if (cursor != tokens_count) {
        Utils::error("PatternMatchingQuery message: parse error in tokens");
    }

    while (! execution_stack.empty()) {
        cursor = execution_stack.top();
        if (tokens[cursor] == "NODE") {
            element_stack.push(new Node(tokens[cursor + 1], tokens[cursor + 2]));
        } else if (tokens[cursor] == "VARIABLE") {
            element_stack.push(new Variable(tokens[cursor + 1]));
        } else if (tokens[cursor] == "LINK") {
            element_stack.push(build_link(tokens, cursor, element_stack));
        } else if (tokens[cursor] == "LINK_TEMPLATE") {
            element_stack.push(build_link_template(tokens, cursor, element_stack));
        } else {
            Utils::error("Invalid token " + tokens[cursor] + " in PatternMatchingQuery message");
        }
        execution_stack.pop();
    }

    if (element_stack.size() != 1) {
        Utils::error("PatternMatchingQuery message: parse error in tokens (trailing elements)");
    }
    this->root_query_element = element_stack.top();
    element_stack.pop();
    cout << "XXXXXXXX COSNTRUCTOR PatternMatchingQuery END " << std::to_string((unsigned long) this) << endl;
}

void PatternMatchingQuery::act(shared_ptr<MessageFactory> node) {
    auto das_node = dynamic_pointer_cast<DASNode>(node);

    cout << "XXXXXXXX PatternMatchingQuery::act() BEGIN " << std::to_string((unsigned long) this) << endl;
    cout << "XXXXXXXX PatternMatchingQuery::act() building sink this->requestor_id: " << this->requestor_id << endl;
    // TODO XXX Remove memory leak
    RemoteSink *remote_sink = new RemoteSink(
        this->root_query_element,
        das_node->next_query_id(),
        this->requestor_id);
    cout << "XXXXXXXX PatternMatchingQuery::act() END " << std::to_string((unsigned long) this) << endl;
}
