#include "func_server.h"

#include <iostream>
#include <memory>
#include <string>

#include <glog/logging.h>
#include <grpcpp/grpcpp.h>

#include "func.grpc.pb.h"
#include "func_infra.h"

using func::EventNum;
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

Status FuncImpl::Hook(ServerContext* context, const HookRequest* request,
                      HookReply* response) {
  Status status = Status::OK;
  int event = request->event_type();
  bool isInRange = event < EventType::EVENT_MAX && event > EventType::EVENT_MIN;
  if (isInRange) {
    EventType e = static_cast<EventType>(event);
    if (func_.IsHooked(e)) {
      grpc::string error_string("The event type is already hooked!");
      status = Status(grpc::StatusCode::ALREADY_EXISTS, error_string);

      LOG(ERROR) << "The event type is already hooked!" << std::endl;
    } else {
      func_.Hook(e);

      LOG(INFO) << "Event Type:" << event << "is hooked!";
    }
  } else {
    grpc::string error_string("The event type is out of range");
    status = Status(grpc::StatusCode::OUT_OF_RANGE, error_string);
    LOG(ERROR) << "The event type is out of range!" << std::endl;
  }
  return status;
}

Status FuncImpl::Unhook(ServerContext* context, const UnhookRequest* request,
                        UnhookReply* response) {
  Status status = Status::OK;
  int event = request->event_type();
  bool isInRange = event < EventType::EVENT_MAX && event > EventType::EVENT_MIN;
  if (isInRange) {
    EventType e = static_cast<EventType>(event);
    if (!func_.IsHooked(e)) {
      grpc::string error_string("The event type is not hooked!");
      status = Status(grpc::StatusCode::NOT_FOUND, error_string);

      LOG(ERROR) << "The event type is not found!" << std::endl;
    } else {
      func_.Unhook(e);

      LOG(INFO) << "Event Type:" << event << "is unhooked!";
    }
  } else {
    grpc::string error_string("The event type is out of range");
    status = Status(grpc::StatusCode::OUT_OF_RANGE, error_string);
    LOG(ERROR) << "The event type is out of range!" << std::endl;
  }
  return status;
}

Status FuncImpl::Event(ServerContext* context, const EventRequest* request,
                       EventReply* response) {
  Status status = Status::OK;
  int event = request->event_type();
  bool isInRange = event < EventType::EVENT_MAX && event > EventType::EVENT_MIN;
  if (isInRange) {
    EventType e = static_cast<EventType>(event);
    google::protobuf::Message* message;
    request->payload().UnpackTo(message);
    func_.EventHandler(e, message);
  } else {
    grpc::string error_string("The event type is out of range");
    status = Status(grpc::StatusCode::OUT_OF_RANGE, error_string);
    LOG(ERROR) << "The event type is out of range!" << std::endl;
  }
  return status;
}

// start the server
void RunServer() {
  // server running on port 50001
  std::string server_address("0.0.0.0:50000");
  FuncImpl service;
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  LOG(INFO) << "Server listening on " << server_address << std::endl;
  server->Wait();
}

int main(int argc, char** argv) {
  // start logging
  google::InitGoogleLogging(argv[0]);
  LOG(INFO) << "Run Server" << std::endl;
  RunServer();
  return 0;
}