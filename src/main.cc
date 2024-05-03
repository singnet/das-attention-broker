#include <iostream>
#include <string>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "common.pb.h"
#include "attention_broker.grpc.pb.h"
#include "attention_broker.pb.h"

#include "attention_broker.h"

void run_server(uint16_t port) {
  std::string server_address = "0.0.0.0:50051";
  AttentionBrokerServer service;
  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

int main(int argc, char* argv[]) {
    run_server(50051);
    return 0;
}
