#include "Sink.h"

using namespace query_element;

// --------------------------------------------------------------------------------
// Public methods

Sink::Sink(QueryElement *precedent, const string &id) {
    this->precedent = precedent;
    this->id = id;
    setup_buffers();
}

Sink::~Sink() {
    this->input_buffer->graceful_shutdown();
}

void Sink::setup_buffers() {
    if (this->subsequent_id != "") {
        Utils::error("Invalid non-empty subsequent id: " + this->subsequent_id);
    }
    if (this->id == "") {
        Utils::error("Invalid empty id");
    }
    cout << "XXXXX sink server id: " << this->id << endl;
    this->input_buffer = shared_ptr<QueryNode>(new QueryNodeServer(this->id));
    this->precedent->subsequent_id = this->id;
    this->precedent->setup_buffers();
}

void Sink::graceful_shutdown() {
    this->input_buffer->graceful_shutdown();
}
