#ifndef _ATOM_SPACE_NODE_MESSAGEBROKER_H
#define _ATOM_SPACE_NODE_MESSAGEBROKER_H

#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include "atom_space_node.grpc.pb.h"
#include "Message.h"
#include "RequestQueue.h"

using namespace std;
using namespace commons;

namespace atom_space_node {

enum class MessageBrokerType {
    GRPC
};

class AtomSpaceNode;

/**
 *
 */
class MessageBroker {

public:

    static MessageBroker *factory(MessageBrokerType instance_type, MessageFactory *host_node, string &node_id);
    virtual ~MessageBroker();
    virtual void add_peer(const string &peer_id);

    // Public abstract API

    virtual void join_network() = 0;
    virtual void broadcast(string &command, vector<string> &args);

protected:

    MessageBroker(MessageFactory *host_node, string &node_id);
    MessageFactory *host_node;
    vector<string> peers;
    mutex peers_mutex;
    string node_id;
};

class SynchronousGRPC : public MessageBroker, public dasproto::AtomSpaceNode::Service {

public:

    SynchronousGRPC(MessageFactory *host_node, string &node_id);
    ~SynchronousGRPC();

    void add_peer(const string &peer_id);

    // Public MessageBroker API

    void join_network();
    void broadcast(string &command, vector<string> &args);

    // Public GRPC API

    Status ping(ServerContext* grpc_context, const dasproto::Empty* request, dasproto::Ack* reply) override;
    Status execute_message(ServerContext* grpc_context, const dasproto::MessageData* request, dasproto::Empty* reply) override;

private:

    static unsigned int GRPC_MESSAGE_BROKER_PORT;
    static unsigned int MESSAGE_THREAD_COUNT;
    thread *grpc_thread;
    vector<thread *> inbox_threads;
    vector<thread *> outbox_threads;
    RequestQueue incoming_messages; // Thread safe
    RequestQueue outgoing_messages; // Thread safe
    unordered_map<string, unique_ptr<dasproto::AtomSpaceNode::Stub>> grpc_stub;
    grpc::ClientContext grpc_context;

    void grpc_thread_method();
    void inbox_thread_method();
    void outbox_thread_method();
};


} // namespace atom_space_node

#endif // _ATOM_SPACE_NODE_MESSAGEBROKER_H

