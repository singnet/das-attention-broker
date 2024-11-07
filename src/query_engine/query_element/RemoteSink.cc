#include "RemoteSink.h"

using namespace query_element;

// -------------------------------------------------------------------------------------------------
// Public methods

RemoteSink::RemoteSink(
    QueryElement *precedent, 
    const string &local_id,
    const string &remote_id,
    bool delete_precedent_on_destructor) : 
    Sink(precedent, "RemoteSink(" + precedent->id + ")", delete_precedent_on_destructor) {

    this->local_id = local_id;
    this->remote_id = remote_id;
    this->queue_processor = NULL;
}

RemoteSink::~RemoteSink() {
    graceful_shutdown();
}

void RemoteSink::setup_buffers() {
    cout << "XXXXX RemoteSink::setup_buffers(): " << (unsigned long) this << endl;
    cout << "XXXXX local_id: " << local_id << endl;
    cout << "XXXXX remote_id: " << remote_id << endl;
    //Sink::setup_buffers();
    this->remote_output_buffer = shared_ptr<QueryNode>(new QueryNodeClient(this->local_id, this->remote_id));
    this->queue_processor = new thread(&RemoteSink::queue_processor_method, this);
}

void RemoteSink::graceful_shutdown() {
    Sink::graceful_shutdown();
    set_flow_finished();
    if (this->queue_processor != NULL) {
        this->queue_processor->join();
    }
    this->remote_output_buffer->graceful_shutdown();
}

void RemoteSink::queue_processor_method() {
    do {
        if (is_flow_finished() || (this->input_buffer->is_query_answers_finished() && this->input_buffer->is_query_answers_empty())) {
            break;
        }
        bool idle_flag = true;
        DASQueryAnswer *query_answer;
        while ((query_answer = this->input_buffer->pop_query_answer()) != NULL) {
            this->remote_output_buffer->add_query_answer(query_answer);
            idle_flag = false;
        }
        if (idle_flag) {
            Utils::sleep();
        }
    } while (true);
    this->remote_output_buffer->query_answers_finished();
    set_flow_finished();
}
