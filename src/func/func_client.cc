#include "func_client.h"

#include <string>
#include <vector>
#include <memory>

#include <grpcpp/grpcpp.h>
#include <google/protobuf/message.h>

#include "func.grpc.pb.h"
#include "warble.grpc.pb.h"
#include "func_infra.h"

using func::FuncService;
using func::EventReply;
using func::EventRequest;
using func::EventType;
using func::FuncService;
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
    : stub_(FuncService::NewStub(chnanel)) {}

Status FuncClient::Hook(const EventType& e) {
  HookRequest request;
  request.set_event_type(e);
  HookReply reply;
  ClientContext context;
  Status status  = stub_->Hook(&context, request, &reply);
  if (status.ok()) {
    LOG(INFO) << "Hook(" << e << ") status: OK " << std::endl;
  } else {
    LOG(ERROR) << status.error_code() << ": " << status.error_message()
               << std::endl;
  }
  return status;
}

Status FuncClient::Unhook(const EventType& e) {
  UnhookRequest request;
  request.set_event_type(e);
  UnhookReply reply;
  ClientContext context;
  Status status  = stub_->Unhook(&context, request, &reply);
  if (status.ok()) {
    LOG(INFO) << "Unhook(" << e << ") status: OK " << std::endl;
  } else {
    LOG(ERROR) << status.error_code() << ": " << status.error_message()
               << std::endl;
  }
  return status;
}

Status FuncClient::Event(const EventType& e, const Message& input, Message& output) {
  EventRequest request;
  request.set_event_type(e);
  Any payload;
  payload.PackFrom(input);
  request.set_payload(payload);
  EventReply reply;
  ClientContext context;
  Status status = stub_->Hook(&context, request, &reply);
  if (status.ok()) {
    LOG(INFO) << "Evant(" << e << ") status: OK " << std::endl;
  } else {
    LOG(ERROR) << status.error_code() << ": " << status.error_message()
               << std::endl;
  }
  return status;
}

