#ifndef _QUERY_NODE_QUERYNODE_H
#define _QUERY_NODE_QUERYNODE_H

#include <string>
#include <thread>
#include "AtomSpaceNode.h"
#include "RequestQueue.h"
#include "DASQueryAnswer.h"

using namespace std;
using namespace atom_space_node;
using namespace query_engine;

namespace query_node {

/**
 *
 */
class QueryNode : public AtomSpaceNode {

public:

    QueryNode(
        const string &node_id, 
        bool is_server, 
        MessageBrokerType messaging_backend = MessageBrokerType::RAM);
    virtual ~QueryNode();
    virtual shared_ptr<Message> message_factory(string &command, vector<string> &args);
    virtual void graceful_shutdown();
    bool is_shutting_down();
    void query_answers_finished();
    bool is_query_answers_finished();
    void add_query_answer(DASQueryAnswer *query_answer);
    DASQueryAnswer *pop_query_answer();
    bool is_query_answers_empty();
    virtual void query_answer_processor_method() = 0;

    static string QUERY_ANSWER_FLOW_COMMAND;
    static string QUERY_ANSWERS_FINISHED_COMMAND;

protected:

    RequestQueue query_answer_queue;
    thread *query_answer_processor;

private:

    bool is_server;
    bool shutdown_flag;
    mutex shutdown_flag_mutex;
    bool query_answers_finished_flag;
    mutex query_answers_finished_flag_mutex;
};

class QueryNodeServer : public QueryNode {

public:

    QueryNodeServer(const string &node_id);
    virtual ~QueryNodeServer();

    void node_joined_network(const string &node_id);
    string cast_leadership_vote();
    void query_answer_processor_method();
};

class QueryNodeClient : public QueryNode {

public:

    QueryNodeClient(const string &node_id, const string &server_id);
    virtual ~QueryNodeClient();

    void node_joined_network(const string &node_id);
    string cast_leadership_vote();
    void query_answer_processor_method();

private:

    string server_id;
};

class QueryAnswerFlow : public Message {

public:

    QueryAnswerFlow(string command, vector<string> args);
    void act(shared_ptr<MessageFactory> node);

private:

    vector<DASQueryAnswer *> query_answers;
};

class QueryAnswersFinished : public Message {

public:

    QueryAnswersFinished(string command, vector<string> args);
    void act(shared_ptr<MessageFactory> node);
};

} // namespace query_node

#endif // _QUERY_NODE_QUERYNODE_H
