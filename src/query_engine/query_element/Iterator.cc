#include "Iterator.h"

using namespace query_element;

// -------------------------------------------------------------------------------------------------
// Public methods

Iterator::Iterator(QueryElement *precedent, bool delete_precedent_on_destructor) : Sink(precedent, "Iterator(" + precedent->id + ")", delete_precedent_on_destructor) {
    cout << "XXXXX CONSTRUCTOR Iterator::Iterator() BEGIN: " << (unsigned long) this << endl;
    cout << "XXXXX CONSTRUCTOR Iterator::Iterator() END: " << (unsigned long) this << endl;
}

Iterator::~Iterator() {
    cout << "XXXXX DESTRUCTOR Iterator::Iterator() BEGIN: " << (unsigned long) this << endl;
    cout << "XXXXX DESTRUCTOR Iterator::Iterator() END: " << (unsigned long) this << endl;
}

bool Iterator::finished() {
    // The order of the AND clauses below matters
    return (
        this->input_buffer->is_query_answers_empty() && 
        this->input_buffer->is_query_answers_finished());
}

DASQueryAnswer *Iterator::pop() {
    return (DASQueryAnswer *) this->input_buffer->pop_query_answer();
}
