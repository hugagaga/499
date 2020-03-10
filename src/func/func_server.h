#ifndef WARBLE_FUNC_SERVER_H_
#define WARBLE_FUNC_SERVER_H_

#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "func.grpc.pb.h"
#include "func_infra.h"


using func::EventReply;
using func::EventRequest;
using func::FuncService;
using func::HookReply;
using func::HookRequest;
using func::UnhookReply;
using func::UnhookRequest;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

// Func service implimentations
class FuncImpl final : public FuncService::Service {
 public:
  // Initialization
  void Init();
  // Call FuncInfra::Hook(EventType) function
  Status Hook(ServerContext* context, const HookRequest* request,
              HookReply* response) override;
  // Call FuncInfra::Unook(EventType) function
  Status Unhook(ServerContext* context, const UnhookRequest* request,
                UnhookReply* response) override;
  // Call Kvmap::Remove(key) function
  Status Event(ServerContext* context, const EventRequest* request,
               EventReply* response) override;

 private:
  // FuncInfra object of Func Faas functionality
  func::FuncInfra func_;
};

// Run the server
void RunServer();

#endif  // WARBLE_FUNC_SERVER_H_