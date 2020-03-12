#ifndef WARBLE_FUNC_CLIENT_H_
#define WARBLE_FUNC_CLIENT_H_

#include <memory>
#include <string>
#include <vector>

#include <grpcpp/grpcpp.h>
#include <google/protobuf/message.h>

#include "warble.grpc.pb.h"
#include "func.grpc.pb.h"
#include "func_infra.h"

using grpc::Channel;
using grpc::Status;
using func::FuncService;
using func::EventType;
using google::protobuf::Message;
using google::protobuf::Any;


// Client class for clients to use key-value store service on the server
class FuncClient {
 public:
  FuncClient(std::shared_ptr<Channel> channel);
  // Hook a function for the event Type
  Status Hook(const EventType& e);
  // Unhook a function for the event Type
  Status Unhook(const EventType& e);
  // Execute a function for the event type given the input(s)
  Status Event(const EventType& e, Any* input, Any* output);

 private:
  // FuncService client stub to start RPC call.
  std::unique_ptr<FuncService::Stub> stub_;
};

#endif  // WARBLE_KVSTORE_CLIENT_H_