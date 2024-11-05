#ifndef _QUERY_ELEMENT_OPERATOR_H
#define _QUERY_ELEMENT_OPERATOR_H

#include <mutex>
#include "Source.h"
#include "Sink.h"
#include "DASQueryAnswer.h"

using namespace std;

namespace query_element {

/**
 *
 */
template <unsigned int N>
class Operator : public QueryElement {

public:

    Operator(const array<QueryElement *, N> &clauses) {
        cout << "XXXXX CONSTRUCTOR Operator::Operator() BEGIN: " << (unsigned long) this << endl;
        initialize((QueryElement **) clauses.data());
        cout << "XXXXX CONSTRUCTOR Operator::Operator() END: " << (unsigned long) this << endl;
    }

    Operator(QueryElement **clauses) {
        cout << "XXXXX CONSTRUCTOR Operator::Operator() BEGIN: " << (unsigned long) this << endl;
        initialize(clauses);
        cout << "XXXXX CONSTRUCTOR Operator::Operator() END: " << (unsigned long) this << endl;
    }

    ~Operator() {
        cout << "XXXXX DESTRUCTOR Operator::Operator() BEGIN: " << (unsigned long) this << endl;
        this->graceful_shutdown();
        cout << "XXXXX DESTRUCTOR Operator::Operator() END: " << (unsigned long) this << endl;
    }

    virtual void graceful_shutdown() {
        cout << "XXXXX Operator::graceful_shutdown(): " << (unsigned long) this << endl;
        if (is_shutting_down()) {
            return;
        }
        for (unsigned int i = 0; i < N; i++) {
            this->precedent[i]->graceful_shutdown();
        }
        this->shutdown_flag_mutex.lock();
        this->shutdown_flag = true;
        this->shutdown_flag_mutex.unlock();
        this->output_buffer->graceful_shutdown();
        for (unsigned int i = 0; i < N; i++) {
            this->input_buffer[i]->graceful_shutdown();
        }
    }

    bool is_shutting_down() {
        bool answer;
        this->shutdown_flag_mutex.lock();
        answer = this->shutdown_flag;
        this->shutdown_flag_mutex.unlock();
        return answer;
    }

    virtual void setup_buffers() {
        cout << "XXXXX Operator::setup_buffers(): " << (unsigned long) this << endl;
        if (this->subsequent_id == "") {
            Utils::error("Invalid empty parent id");
        }
        if (this->id == "") {
            Utils::error("Invalid empty id");
        }

        this->output_buffer = shared_ptr<QueryNodeClient>(new QueryNodeClient(this->id, this->subsequent_id));
        cout << "XXXXX operator client id: " << this->id << endl;
        cout << "XXXXX operator subsequent id: " << this->subsequent_id << endl;
        string server_node_id;
        for (unsigned int i = 0; i < N; i++) {
            server_node_id = this->id + "_" + to_string(i);
            cout << "XXXXX operator server id: " << server_node_id << endl;
            this->input_buffer[i] = shared_ptr<QueryNodeServer>(new QueryNodeServer(server_node_id));
            this->precedent[i]->subsequent_id = server_node_id;
            this->precedent[i]->setup_buffers();
        }

    }

protected:

    QueryElement *precedent[N];
    shared_ptr<QueryNodeServer> input_buffer[N];
    shared_ptr<QueryNodeClient> output_buffer;

private:

    bool shutdown_flag;
    mutex shutdown_flag_mutex;

    void initialize(QueryElement **clauses) {
        this->shutdown_flag = false;
        if (N > MAX_NUMBER_OF_OPERATION_CLAUSES) {
            Utils::error("Operation exceeds max number of clauses: " + to_string(N));
        }
        for (unsigned int i = 0; i < N; i++) {
            precedent[i] = clauses[i];
        }
    }
};

} // namespace query_element

#endif // _QUERY_ELEMENT_OPERATOR_H
