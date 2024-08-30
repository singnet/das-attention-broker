#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>


#include "common.pb.h"
#include "atom_space_node.grpc.pb.h"
#include "atom_space_node.pb.h"

#include "Utils.h"
#include "MessageBroker.h"

using namespace atom_space_node;

unsigned int SynchronousGRPC::GRPC_MESSAGE_BROKER_PORT = 32501;
unsigned int SynchronousGRPC::MESSAGE_THREAD_COUNT = 10;

MessageBroker::MessageBroker(MessageFactory *host_node, string &node_id) {
    this->host_node = host_node;
    this->node_id = node_id;
}

// ------------------------------------------------------------------------------------------------
// Public methods

MessageBroker *MessageBroker::factory(MessageBrokerType instance_type, MessageFactory *host_node, string &node_id) {
    switch (instance_type) {
        case MessageBrokerType::GRPC : {
            return new SynchronousGRPC(host_node, node_id);
        }
        default: {
            Utils::error("Invalid MessageBrokerType: " + to_string((int) instance_type));
            return NULL; // to avoid warnings
        }
    }
}

MessageBroker::~MessageBroker() {
}

void MessageBroker::add_peer(const string &peer_id) {
    peers_mutex.lock();
    peers.push_back(peer_id);
    peers_mutex.unlock();
}

SynchronousGRPC::SynchronousGRPC(MessageFactory *host_node, string &node_id) : MessageBroker(host_node, node_id) {
}

SynchronousGRPC::~SynchronousGRPC() {
}

grpc::Status SynchronousGRPC::ping(grpc::ServerContext* grpc_context, const dasproto::Empty* request, dasproto::Ack* reply) {
    reply->set_msg("PING");
    return grpc::Status::OK;
}

grpc::Status SynchronousGRPC::execute_message(grpc::ServerContext* grpc_context, const dasproto::MessageData* request, dasproto::Empty* reply) {
    this->incoming_messages.enqueue((void *) request);
    return grpc::Status::OK;
}

void SynchronousGRPC::grpc_thread_method() {
    std::string server_address = "localhost:" + to_string(GRPC_MESSAGE_BROKER_PORT);
    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(this);
    std::cout << "SynchronousGRPC listening on " << server_address << std::endl;
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    server->Wait();
}

void SynchronousGRPC::inbox_thread_method() {
    do {
        void *request = this->incoming_messages.dequeue();
        if (request != NULL) {
            dasproto::MessageData *message_data = (dasproto::MessageData *) request;
            if (message_data->is_broadcast()) {
                unordered_set<string> visited;
                int num_visited = message_data->visited_recipients_size();
                for (int i = 0; i < num_visited; i++) {
                    visited.insert(message_data->visited_recipients(i));
                }
                if (visited.find(this->node_id) != visited.end()) {
                    continue;
                }
                message_data->add_visited_recipients(this->node_id);
                dasproto::Empty reply;
                grpc::ClientContext context;
                this->peers_mutex.lock();
                for (auto target: this->peers) {
                    if (visited.find(target) == visited.end()) {
                        this->grpc_stub[target]->execute_message(&context, *message_data, &reply);
                    }
                }
                this->peers_mutex.unlock();
            }
            string command = message_data->command();
            vector<string> args;
            int num_args = message_data->args_size();
            for (int i = 0; i < num_args; i++) {
                args.push_back(message_data->args(i));
            }
            Message *message = this->host_node->message_factory(command, args);
            message->act((AtomSpaceNode *) this->host_node);

        } else {
            Utils::sleep();
        }
    } while (true);
}

void SynchronousGRPC::outbox_thread_method() {
    do {
        Utils::sleep();
    } while (true);
}

void SynchronousGRPC::join_network() {
    this->grpc_thread = new std::thread(&SynchronousGRPC::grpc_thread_method, this);
    for (unsigned int i = 0; i < MESSAGE_THREAD_COUNT; i++) {
        this->inbox_threads.push_back(new thread(
            &SynchronousGRPC::inbox_thread_method,
            this));
        this->outbox_threads.push_back(new thread(
            &SynchronousGRPC::outbox_thread_method,
            this));
    }
}

void SynchronousGRPC::add_peer(const string &peer_id) {
    MessageBroker::add_peer(peer_id);
    auto channel = grpc::CreateChannel(peer_id, grpc::InsecureChannelCredentials());
    this->grpc_stub[peer_id] = dasproto::AtomSpaceNode::NewStub(channel);
}

void SynchronousGRPC::broadcast(string &command, vector<string> &args) {
    dasproto::Empty reply;
    grpc::ClientContext context;
    this->peers_mutex.lock();
    for (auto peer_id: this->peers) {
        dasproto::MessageData message_data;
        message_data.set_command(command);
        for (auto arg: args) {
            message_data.add_args(arg);
        }
        message_data.set_sender(this->node_id);
        message_data.set_is_broadcast(true);
        this->grpc_stub[peer_id]->execute_message(&context, message_data, &reply);
    }
    this->peers_mutex.unlock();
}
