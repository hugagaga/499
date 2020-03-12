#include "func_server.h"

#include <iostream>
#include <memory>
#include <string>
#include <optional>

#include <glog/logging.h>
#include <google/protobuf/message.h>
#include <grpcpp/grpcpp.h>

#include "func.grpc.pb.h"
#include "func_infra.h"

using func::EventReply;
using func::EventRequest;
using func::EventType;
using func::FuncService;
using func::HookReply;
using func::HookRequest;
using func::UnhookReply;
using func::UnhookRequest;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using google::protobuf::Any;

void FuncImpl::Init() {
  func_.Init();
}

Status FuncImpl::Hook(ServerContext *context, const HookRequest *request,
                      HookReply *response) {
  Status status = Status::OK;
  int event = request->event_type();
  EventType e = static_cast<EventType>(event);
  if (func_.IsHooked(e)) {
    grpc::string error_string("The event type is already hooked!");
    status = Status(grpc::StatusCode::ALREADY_EXISTS, error_string);

    LOG(ERROR) << "The event type is already hooked!" << std::endl;
  } else {
    func_.Hook(e);
    LOG(INFO) << "Event Type:" << event << "is hooked!";
  }
  return status;
}

Status FuncImpl::Unhook(ServerContext *context, const UnhookRequest *request,
                        UnhookReply *response) {
  Status status = Status::OK;
  int event = request->event_type();
  EventType e = static_cast<EventType>(event);
  if (!func_.IsHooked(e)) {
    grpc::string error_string("The event type is not hooked!");
    status = Status(grpc::StatusCode::NOT_FOUND, error_string);

    LOG(ERROR) << "The event type is not found!" << std::endl;
  } else {
    func_.Unhook(e);

    LOG(INFO) << "Event Type:" << event << "is unhooked!";
  }
  return status;
}

Status FuncImpl::Event(ServerContext *context, const EventRequest *request,
                       EventReply *response) {
  Status status = Status::OK;
  int event = request->event_type();
  LOG(INFO) << "Start to handle event" << event << std::endl;
  EventType e = static_cast<EventType>(event);
  google::protobuf::Any input = request->payload();
  std::optional<Any>output_opt;
  func_.EventHandler(e, input, output_opt);
  if(!output_opt.has_value()) {
    grpc::string error_string("Can not handle Event!");
    LOG(ERROR) << "Can not handle Event " << event << std::endl;
    status = Status(grpc::StatusCode::INVALID_ARGUMENT, error_string);
  } else {
    Any* output = new Any(output_opt.value());
    response->set_allocated_payload(output);
  }
  return status;
}

// start the server
void RunServer() {
  // server running on port 50001
  std::string server_address("0.0.0.0:50000");
  FuncImpl service;
  service.Init();
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

int main(int argc, char **argv) {
  // start logging
  FLAGS_log_dir = "/499/log";
  google::InitGoogleLogging(argv[0]);
  LOG(INFO) << "Run Server" << std::endl;
  RunServer();
  return 0;
}