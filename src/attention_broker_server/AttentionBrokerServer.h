#ifndef _ATTENTION_BROKER_SERVER_ATTENTIONBROKERSERVER_H
#define _ATTENTION_BROKER_SERVER_ATTENTIONBROKERSERVER_H

#include <string>
#include <unordered_map>
#include "attention_broker.grpc.pb.h"
#include "RequestQueue.h"
#include "WorkerThreads.h"
#include "HebbianNetwork.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using das::AttentionBroker;

namespace attention_broker_server {

/**
 * GRPC server which actually listens to a PORT.
 *
 * This class implements the GRPC server which listens to a PORT and answer
 * the public RPC API defined in the protobuf.
 *
 * Some parameters related to the stimulus spreading algorithms are also
 * stored in this class as static fields. Detailed description of them are
 * provided in StimulsSpreader. They are defined here because we may want
 * to allow them to be modified by some homeostasis process running independently
 * from StimulusSpreading.
 */
class AttentionBrokerServer final: public AttentionBroker::Service {

    public:

        /**
         * Basic no-parameters constructor.
         * Creates and initializes two request queues. One for stimuli spreading and
         * another one for atom correlation. Worker threads to process such queues are
         * also created inside a WorkerThread object.
         *
         * Different contexts are represented using different HebianNetwork objects. New
         * contexts are created by caller's request but a default GLOBAL context is created
         * here to be used whenever the caller don't specify a context.
         */
        AttentionBrokerServer();

        /**
         * Destructor.
         *
         * Gracefully shutdown the GRPC server by stopping accepting new requests (any new request
         * received after starting a shutdown process is denied and an error is returned to the
         * caller) and waiting for all requests currently in the queues to be processed. Once all
         * queues are empty, all worker threads are stopped and the queues and all other state structures
         * are destroyed.
         */
        ~AttentionBrokerServer();

        static const unsigned int WORKER_THREADS_COUNT = 10; /// Number of working threads.

        // Stimuli spreading parameters
        static const double RENT_RATE; /// double in [0..1] range.
        static const double SPREADING_RATE_LOWERBOUND; /// double in [0..1] range.
        static const double SPREADING_RATE_UPPERBOUND; /// double in [0..1] range.
        static const string GLOBAL_CONTEXT; /// Name of global context.

        // RPC API
          
        /**
         * Used by caller to check if AttentionBroker is running.
         *
         * @param grpc_context GRPC context object.
         * @param request Empty request.
         * @param reply The message which will be send back to the caller with a simple ACK.
         *
         * @return GRPC status OK if request were properly processed or CANCELLED otherwise.
         */
        Status ping(ServerContext* grpc_context, const das::Empty* request, das::Ack* reply) override;

        /**
         * Spread stimuli according to the passed request.
         *
         * Boost importance of passed atoms and run one cycle of stimuli spreading. The algorithm is explained
         * in StimulusSpreader.
         *
         * @param grpc_context GRPC context object.
         * @param request The request contains a list of handles of the atoms which should have the boost in
         * importance as well as an associated integer indicating the proportion in which the boost should
         * happen related to the other atoms in the same request.
         * @param reply The message which will be send back to the caller with a simple ACK.
         *
         * @return GRPC status OK if request were properly processed or CANCELLED otherwise.
         */
        Status stimulate(ServerContext* grpc_context, const das::HandleCount* request, das::Ack* reply) override;

        /**
         * Correlates atoms passed in the request.
         *
         * @param grpc_context GRPC context object.
         * @param request The request contains a list of handles of the atoms which should be correlated.
         * @param reply The message which will be send back to the caller with a simple ACK.
         *
         * @return GRPC status OK if request were properly processed or CANCELLED otherwise.
         */
        Status correlate(ServerContext* grpc_context, const das::HandleList* request, das::Ack* reply) override;

        // Other public methods

        /**
         * Gracefully stop this GRPC server.
         *
         * Gracefully shutdown the GRPC server by stopping accepting new requests (any new request
         * received after starting a shutdown process is denied and an error is returned to the
         * caller) and waiting for all requests currently in the queues to be processed.
         */
        void graceful_shutdown(); /// Gracefully stop this GRPC server.

    private:

        bool rpc_api_enabled = true;
        RequestQueue *stimulus_requests;
        RequestQueue *correlation_requests;
        WorkerThreads *worker_threads;
        unordered_map<string, HebbianNetwork *> hebbian_network;

        HebbianNetwork *select_hebbian_network(const string &context);
};

} // namespace attention_broker_server

#endif // _ATTENTION_BROKER_SERVER_ATTENTIONBROKERSERVER_H
