#include "RemoteSink.h"

using namespace query_element;

// -------------------------------------------------------------------------------------------------
// Constructors and destructors

RemoteSink::RemoteSink(
    QueryElement *precedent, 
    const string &local_id,
    const string &remote_id,
    bool delete_precedent_on_destructor) : 
    Sink(precedent, "RemoteSink(" + precedent->id + ")", delete_precedent_on_destructor) {


    cout << "XXXXXXXX COSNTRUCTOR RemoteSink BEGIN " << std::to_string((unsigned long) this) << endl;
    cout << "XXXXXXXX COSNTRUCTOR RemoteSink local_id: " << local_id << " remote_id: " << remote_id << endl;
    this->local_id = local_id;
    this->remote_id = remote_id;
    this->queue_processor = NULL;
    RemoteSink::setup_buffers();
    cout << "XXXXXXXX COSNTRUCTOR RemoteSink END " << std::to_string((unsigned long) this) << endl;
}

RemoteSink::~RemoteSink() {
    graceful_shutdown();
}

// -------------------------------------------------------------------------------------------------
// Public methods

void RemoteSink::setup_buffers() {
    cout << "XXXXXXXX RemoteSink::setup_buffers() BEGIN " << std::to_string((unsigned long) this) << endl;

    this->remote_output_buffer = shared_ptr<QueryNode>(new QueryNodeClient(
        this->local_id, 
        this->remote_id, 
        MessageBrokerType::GRPC));
    this->queue_processor = new thread(&RemoteSink::queue_processor_method, this);
    cout << "XXXXXXXX RemoteSink::setup_buffers() END " << std::to_string((unsigned long) this) << endl;
}

void RemoteSink::graceful_shutdown() {
    Sink::graceful_shutdown();
    set_flow_finished();
    if (this->queue_processor != NULL) {
        this->queue_processor->join();
    }
    this->remote_output_buffer->graceful_shutdown();
}

// -------------------------------------------------------------------------------------------------
// Private methods

void RemoteSink::queue_processor_method() {
    do {
        if (is_flow_finished() || 
           (this->input_buffer->is_query_answers_finished() && 
            this->input_buffer->is_query_answers_empty())) {

            break;
        }
        bool idle_flag = true;
        QueryAnswer *query_answer;
        while ((query_answer = this->input_buffer->pop_query_answer()) != NULL) {
            this->remote_output_buffer->add_query_answer(query_answer);
            idle_flag = false;
        }
        if (idle_flag) {
            Utils::sleep();
        }
    } while (true);
    cout << "XXXXXXXXXXX XXXXXXXXXXXXX XXXXXXXXXXXXXXX XXXXXXXXXXXXXX RemoteSink FINISHED" << endl;
    this->remote_output_buffer->query_answers_finished();
    set_flow_finished();
}
