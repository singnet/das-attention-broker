#ifndef _ATTENTION_BROKER_SERVER_STIMULUSSPREADER_H
#define _ATTENTION_BROKER_SERVER_STIMULUSSPREADER_H

#include "attention_broker.grpc.pb.h"
#include "Utils.h"
#include "HebbianNetwork.h"

using namespace std;

namespace attention_broker_server {

enum class StimulusSpreaderType {
    TOKEN
};

/**
 *
 */
class StimulusSpreader {

public:

    static StimulusSpreader *factory(StimulusSpreaderType instance_type);
    virtual ~StimulusSpreader();

    virtual void spread_stimuli(das::HandleCount *request) = 0;

protected:

    StimulusSpreader();

private:

};

class TokenSpreader: public StimulusSpreader {

public:

    TokenSpreader();
    ~TokenSpreader();

    typedef struct {
        ImportanceType rent_rate;
        ImportanceType total_rent;
        HandleTrie *importance_changes;
        unsigned int largest_arity;
        ImportanceType spreading_rate_lowerbound;
        ImportanceType spreading_rate_range_size;
        ImportanceType to_spread;
        double sum_weights;
    } StimuliData;

    class ImportanceChanges: public HandleTrie::TrieValue {
        public:
            ImportanceType rent;
            ImportanceType wages;
            ImportanceChanges(ImportanceType r, ImportanceType w) {
                rent = r;
                wages = w;
            }
            void merge(TrieValue *other) {
                rent += ((ImportanceChanges *) other)->rent;
                wages += ((ImportanceChanges *) other)->wages;
            }
    };

    void spread_stimuli(das::HandleCount *request);
    void distribute_wages(das::HandleCount *handle_count, ImportanceType &total_to_spread, StimuliData *data);

};

} // namespace attention_broker_server

#endif // _ATTENTION_BROKER_SERVER_STIMULUSSPREADER_H
