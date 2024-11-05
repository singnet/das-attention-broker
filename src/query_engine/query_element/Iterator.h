#ifndef _QUERY_ELEMENT_ITERATOR_H
#define _QUERY_ELEMENT_ITERATOR_H

#include "Sink.h"
#include "DASQueryAnswer.h"

using namespace std;
using namespace query_engine;

namespace query_element {

/**
 *
 */
class Iterator : public Sink {

public:

    Iterator(QueryElement *precedent, bool delete_precedent_on_destructor = false);
    ~Iterator();

    bool finished();
    DASQueryAnswer *pop();
};

} // namespace query_element

#endif // _QUERY_ELEMENT_ITERATOR_H
