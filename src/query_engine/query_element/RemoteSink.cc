#include "RemoteSink.h"

using namespace query_element;

// -------------------------------------------------------------------------------------------------
// Constructors and destructors

RemoteSink::RemoteSink(
    QueryElement *precedent, 
    const string &local_id,
    const string &remote_id,
    bool delete_precedent_on_destructor) : 
    Sink(precedent, "RemoteSink(" + precedent->id + ")", delete_precedent_on_destructor, false) {
#ifdef DEBUG
    cout << "RemoteSink::RemoteSink() BEGIN" << endl;
    cout << "RemoteSink::RemoteSink() local_id: " << local_id << endl;
    cout << "RemoteSink::RemoteSink() remote_id: " << remote_id << endl;
#endif

    this->local_id = local_id;
    this->remote_id = remote_id;
    this->queue_processor = NULL;
    RemoteSink::setup_buffers();
    Sink::setup_buffers();
#ifdef DEBUG
    cout << "RemoteSink::RemoteSink() END" << endl;
#endif
}

RemoteSink::~RemoteSink() {
    graceful_shutdown();
}

// -------------------------------------------------------------------------------------------------
// Public methods

void RemoteSink::setup_buffers() {
    this->remote_output_buffer = shared_ptr<QueryNode>(new QueryNodeClient(
        this->local_id, 
        this->remote_id, 
        MessageBrokerType::GRPC));
    Utils::sleep();
    this->queue_processor = new thread(&RemoteSink::queue_processor_method, this);
}

void RemoteSink::graceful_shutdown() {
#ifdef DEBUG
    cout << "RemoteSink::graceful_shutdown() BEGIN" << endl;
#endif
    Sink::graceful_shutdown();
    set_flow_finished();
    if (this->queue_processor != NULL) {
        this->queue_processor->join();
    }
    this->remote_output_buffer->graceful_shutdown();
#ifdef DEBUG
    cout << "RemoteSink::graceful_shutdown() END" << endl;
#endif
}

// -------------------------------------------------------------------------------------------------
// Private methods

void RemoteSink::queue_processor_method() {
#ifdef DEBUG
    cout << "RemoteSink::queue_processor_method() BEGIN" << endl;
#endif
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
#ifdef DEBUG
    cout << "RemoteSink::queue_processor_method() ready to return" << endl;
#endif
    this->remote_output_buffer->query_answers_finished();
    set_flow_finished();
#ifdef DEBUG
    cout << "RemoteSink::queue_processor_method() END" << endl;
#endif
}
