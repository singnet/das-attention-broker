#include "Source.h"

using namespace query_element;

string Source::DEFAULT_ATTENTION_BROKER_PORT = "37007";

// --------------------------------------------------------------------------------
// Public methods

Source::Source(const string &attention_broker_address) {
    this->attention_broker_address = attention_broker_address;
}

Source::Source() : Source("localhost:" + Source::DEFAULT_ATTENTION_BROKER_PORT) {
}

Source::~Source() {
    this->output_buffer->graceful_shutdown();
}

void Source::setup_buffers() {
    if (this->subsequent_id == "") {
        Utils::error("Invalid empty parent id");
    }
    if (this->id == "") {
        Utils::error("Invalid empty id");
    }
    this->output_buffer = shared_ptr<QueryNode>(new QueryNodeClient(this->id, this->subsequent_id));
    cout << "XXXXX source client id: " << this->id << endl;
    cout << "XXXXX source subsequent id: " << this->subsequent_id << endl;

}

void Source::query_answers_finished() {
    cout << "XXXXX Source::query_answers_finished() BEGIN" << endl;
    this->output_buffer->query_answers_finished();
    cout << "XXXXX Source::query_answers_finished() END" << endl;
}

void Source::graceful_shutdown() {
    this->output_buffer->graceful_shutdown();
}
