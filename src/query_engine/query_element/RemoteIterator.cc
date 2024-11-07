#include "RemoteIterator.h"

using namespace query_element;

// -------------------------------------------------------------------------------------------------
// Public methods

RemoteIterator::RemoteIterator(const string &local_id) {
    this->local_id = local_id;
    setup_buffers();
}

RemoteIterator::~RemoteIterator() {
    graceful_shutdown();
}

void RemoteIterator::setup_buffers() {
    cout << "XXXXX RemoteIterator::setup_buffers(): " << (unsigned long) this << endl;
    cout << "XXXXX local_id: " << local_id << endl;
    this->remote_input_buffer = shared_ptr<QueryNode>(new QueryNodeServer(this->local_id));
}

void RemoteIterator::graceful_shutdown() {
    this->remote_input_buffer->graceful_shutdown();
}

bool RemoteIterator::finished() {
    // The order of the AND clauses below matters
    return (
        this->remote_input_buffer->is_query_answers_finished() &&
        this->remote_input_buffer->is_query_answers_empty());
}

DASQueryAnswer *RemoteIterator::pop() {
    return (DASQueryAnswer *) this->remote_input_buffer->pop_query_answer();
}
