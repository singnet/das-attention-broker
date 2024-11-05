#include "Sink.h"

using namespace query_element;

// --------------------------------------------------------------------------------
// Public methods

Sink::Sink(QueryElement *precedent, const string &id, bool delete_precedent_on_destructor) {
    cout << "XXXXX CONSTRUCTOR Sink::Sink() BEGIN: " << (unsigned long) this << endl;
    this->precedent = precedent;
    this->id = id;
    this->delete_precedent_on_destructor = delete_precedent_on_destructor;
    setup_buffers();
    cout << "XXXXX CONSTRUCTOR Sink::Sink() END: " << (unsigned long) this << endl;
}

Sink::~Sink() {
    cout << "XXXXX DESTRUCTOR Sink::Sink() BEGIN: " << (unsigned long) this << endl;
    this->input_buffer->graceful_shutdown();
    if (this->delete_precedent_on_destructor) {
        delete this->precedent;
    }
    cout << "XXXXX DESTRUCTOR Sink::Sink() END: " << (unsigned long) this << endl;
}

void Sink::setup_buffers() {
    cout << "XXXXX Sink::setup_buffers(): " << (unsigned long) this << endl;

    if (this->subsequent_id != "") {
        Utils::error("Invalid non-empty subsequent id: " + this->subsequent_id);
    }
    if (this->id == "") {
        Utils::error("Invalid empty id");
    }
    cout << "XXXXX subsequent id: " << this->subsequent_id << endl;
    cout << "XXXXX id: " << this->id << endl;
    this->input_buffer = shared_ptr<QueryNode>(new QueryNodeServer(this->id));
    cout << "XXXXX 1" << endl;
    this->precedent->subsequent_id = this->id;
    cout << "XXXXX 2" << endl;
    this->precedent->setup_buffers();
    cout << "XXXXX precedent id: " << this->precedent->id << endl;
}

void Sink::graceful_shutdown() {
    cout << "XXXXX Sink::graceful_shutdown(): " << (unsigned long) this << endl;
    this->input_buffer->graceful_shutdown();
    this->precedent->graceful_shutdown();
}
