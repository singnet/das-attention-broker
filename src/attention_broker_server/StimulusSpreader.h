#ifndef _ATTENTION_BROKER_SERVER_STIMULUSSPREADER_H
#define _ATTENTION_BROKER_SERVER_STIMULUSSPREADER_H

#include "attention_broker.grpc.pb.h"
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

    void spread_stimuli(das::HandleCount *request);
    void spread_activation();
    void distribute_wages(HebbianNetwork *network, das::HandleCount *handle_count, ImportanceType &tokens_to_spread);

    class DoubleValue: public HandleTrie::TrieValue {
        public:
            double value;
            DoubleValue(double v) {
                value = v;
            }
            void merge(TrieValue *other) {
            }
    };
};

} // namespace attention_broker_server

#endif // _ATTENTION_BROKER_SERVER_STIMULUSSPREADER_H
