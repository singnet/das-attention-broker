#include "Sink.h"

using namespace query_element;

// ------------------------------------------------------------------------------------------------
// Constructors and destructors

Sink::Sink(QueryElement *precedent, const string &id, bool delete_precedent_on_destructor) {
    cout << "XXXXXXXX COSNTRUCTOR Sink BEGIN " << std::to_string((unsigned long) this) << endl;
    this->precedent = precedent;
    this->id = id;
    this->delete_precedent_on_destructor = delete_precedent_on_destructor;
    setup_buffers();
    cout << "XXXXXXXX COSNTRUCTOR Sink END " << std::to_string((unsigned long) this) << endl;
}

Sink::~Sink() {
    this->input_buffer->graceful_shutdown();
    if (this->delete_precedent_on_destructor) {
        delete this->precedent;
    }
}

// ------------------------------------------------------------------------------------------------
// Public methods

void Sink::setup_buffers() {
    cout << "XXXXXXXX Sink::setup_buffers() BEGIN " << std::to_string((unsigned long) this) << endl;
    if (this->subsequent_id != "") {
        Utils::error("Invalid non-empty subsequent id: " + this->subsequent_id);
    }
    if (this->id == "") {
        Utils::error("Invalid empty id");
    }
    cout << "XXXXXXXX Sink::setup_buffers() 1 " << std::to_string((unsigned long) this) << endl;
    this->input_buffer = shared_ptr<QueryNode>(new QueryNodeServer(this->id));
    this->precedent->subsequent_id = this->id;
    cout << "XXXXXXXX Sink::setup_buffers() 2 " << std::to_string((unsigned long) this) << endl;
    this->precedent->setup_buffers();
    cout << "XXXXXXXX Sink::setup_buffers() END " << std::to_string((unsigned long) this) << endl;
}

void Sink::graceful_shutdown() {
    this->input_buffer->graceful_shutdown();
    this->precedent->graceful_shutdown();
}
