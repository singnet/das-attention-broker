#ifndef _QUERY_ENGINE_DASQUERYANSWER_H
#define _QUERY_ENGINE_DASQUERYANSWER_H

#include <string>
#include "expression_hasher.h"

#define MAX_VARIABLE_NAME_SIZE ((unsigned int) 100)
#define MAX_NUMBER_OF_VARIABLES_IN_QUERY ((unsigned int) 100)
#define MAX_NUMBER_OF_OPERATION_CLAUSES ((unsigned int) 100)

using namespace std;

namespace query_engine {

class Assignment {

    public:

        Assignment();
        ~Assignment();

        string to_string();
        bool assign(const char *label, const char *value);
        const char *get(const char *label);
        bool is_compatible(const Assignment &other);
        void copy_from(const Assignment &other);
        void add_assignments(const Assignment &other);

    private:

        const char *labels[MAX_NUMBER_OF_VARIABLES_IN_QUERY];
        const char *values[MAX_NUMBER_OF_VARIABLES_IN_QUERY];
        unsigned int size;
        bool frozen;
};

/**
 *
 */
class DASQueryAnswer {

public:

    const char *handles[MAX_NUMBER_OF_OPERATION_CLAUSES];
    unsigned int handles_size;
    double importance;
    Assignment assignment;

    DASQueryAnswer(const char *handle, double importance);
    DASQueryAnswer(double importance);
    ~DASQueryAnswer();

    void add_handle(const char *handle);
    bool merge(DASQueryAnswer *other);
    static DASQueryAnswer *copy(DASQueryAnswer *base);

private:

};

} // namespace query_engine

#endif // _QUERY_ENGINE_DASQUERYANSWER_H
