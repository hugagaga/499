#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <glog/logging.h>

#include "kvstore.grpc.pb.h"
#include "kvmap.h"
#include "kvstore_server.h"


using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using kvstore::KeyValueStore;
using kvstore::PutRequest;
using kvstore::PutReply;
using kvstore::GetRequest;
using kvstore::GetReply;
using kvstore::RemoveRequest;
using kvstore::RemoveReply;

Status KeyValueStoreImpl::Put(ServerContext* context, const PutRequest* request, PutReply* response) {
  kvstore_.Put(request->key(), request->value());
  //LOG(INFO) << "Put(" << request->key() << ", " << request->value() << ") status: OK " << std::endl;
  std::cout << "Put(" << request->key() << ", " << request->value() << ") status: OK " << std::endl;
  return Status::OK;
}

Status KeyValueStoreImpl::Get(ServerContext* context, ServerReaderWriter<GetReply, GetRequest>* stream) {
  GetRequest request;
  Status status = Status::OK;
  while (stream->Read(&request)) {
    auto strs = kvstore_.Get(request.key());
      
    GetReply reply;
    if (strs.has_value()) {
      auto values = *strs;
      std::cout << "Get values of key (" << request.key() << ") : " << std::endl;
      for (const std::string& str : values) {
        std::cout << str << " ";
        reply.set_value(str);
        stream->Write(reply);
      }
      std::cout << std::endl;
    } else {
      reply.set_value("");
      stream->Write(reply);
      //LOG(INFO) << "Key is not found!" << std::endl;
      grpc::string error_string("The key is not found!");
      status = Status(grpc::StatusCode::NOT_FOUND, error_string);
    }
  }
  return status;
}

Status KeyValueStoreImpl::Remove(ServerContext* context, const RemoveRequest* request, RemoveReply* response) {
  Status status = Status::OK;
  bool success = kvstore_.Remove(request->key());
  if (!success) {
    grpc::string error_string("The key is not found!");
    status = Status(grpc::StatusCode::NOT_FOUND, error_string);
  }
  return status;
}

//start the server
void RunServer() {
  //server running on port 50001
  std::string server_address("0.0.0.0:50001");
  KeyValueStoreImpl service;
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  //LOG(INFO) << "Server listening on " << server_address << std::endl;
  server->Wait();
}


int main(int argc, char** argv) {
  //start logging
  //google::InitGoogleLogging(argv[0]);
  //LOG(INFO) << "Run Server" << std::endl;
  RunServer();
  return 0;
}