#include "QueryElement.h"

using namespace query_element;

// ------------------------------------------------------------------------------------------------
// Public methods

QueryElement::QueryElement() {
    this->flow_finished = false;
}

QueryElement::~QueryElement() {
}

bool QueryElement::is_terminal() {
    return false;
}

// ------------------------------------------------------------------------------------------------
// Protected methods

void QueryElement::set_flow_finished() {
    this->flow_finished_mutex.lock();
    this->flow_finished = true;
    this->flow_finished_mutex.unlock();
}

bool QueryElement::is_flow_finished() {
    bool answer;
    this->flow_finished_mutex.lock();
    answer = this->flow_finished;
    this->flow_finished_mutex.unlock();
    return answer;
}
