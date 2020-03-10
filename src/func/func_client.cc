#include "func_client.h"

#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include <glog/logging.h>
#include <grpcpp/grpcpp.h>
#include <google/protobuf/message.h>

#include "func.grpc.pb.h"
#include "warble.grpc.pb.h"
#include "func_infra.h"

using func::FuncService;
using func::EventReply;
using func::EventRequest;
using func::EventType;
using func::HookReply;
using func::HookRequest;
using func::UnhookReply;
using func::UnhookRequest;
using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;
using google::protobuf::Message;
using google::protobuf::Any;

FuncClient::FuncClient(std::shared_ptr<Channel> channel)
    : stub_(FuncService::NewStub(channel)) {}

Status FuncClient::Hook(const EventType& e) {
  HookRequest request;
  int event = static_cast<int>(e);
  request.set_event_type(event);
  HookReply reply;
  ClientContext context;
  std::cout << "Start to Hook(" << event << ")" << std::endl;
  LOG(INFO) << "Start to Hook(" << event << ")" << std::endl;
  Status status  = stub_->Hook(&context, request, &reply);
  if (status.ok()) {
    LOG(INFO) << "Hook(" << event << ") status: OK " << std::endl;
  } else {
    LOG(ERROR) << status.error_code() << ": " << status.error_message()
               << std::endl;
  }
  return status;
}

Status FuncClient::HookAll() {
  Hook(EventType::FOLLOW);
  Hook(EventType::PROFILE);
  Hook(EventType::READ);
  Hook(EventType::REGISTER_USER);
  Hook(EventType::WARBLE);
}

Status FuncClient::Unhook(const EventType& e) {
  UnhookRequest request;
  int event = static_cast<int>(e);
  request.set_event_type(event);
  UnhookReply reply;
  ClientContext context;
  Status status  = stub_->Unhook(&context, request, &reply);
  if (status.ok()) {
    LOG(INFO) << "Unhook(" << event << ") status: OK " << std::endl;
  } else {
    LOG(ERROR) << status.error_code() << ": " << status.error_message()
               << std::endl;
  }
  return status;
}

Status FuncClient::Event(const EventType& e, Any* input, Any* output) {
  EventRequest request;
  int event = static_cast<int>(e);
  request.set_event_type(event);
  request.set_allocated_payload(input);
  EventReply reply;
  ClientContext context;
  Status status = stub_->Event(&context, request, &reply);
  if (status.ok()) {
    LOG(INFO) << "Evant(" << event << ") status: OK " << std::endl;
  } else {
    LOG(ERROR) << status.error_code() << ": " << status.error_message()
               << std::endl;
  }
  return status;
}

