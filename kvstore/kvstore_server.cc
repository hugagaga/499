#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "kvstore.grpc.pb.h"

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
using kvstore:: RemoveRequest;
using kvstore:: RemoveReply;

class KeyValueStoreImpl final : public KeyValueStore::Service {
 public:
  Status put(ServerContext* context, const PutRequest* request, PutReply* response) override {
    if (db.find(request->key()) == db.end()){
      db.insert(std::pair<std::string, std::string> (request->key(), request->value()));
    }
    return Status::OK;
  }

  Status get(ServerContext* context, ServerReaderWriter<GetReply, GetRequest>* stream) override {
    GetRequest request;
    while (stream->Read(&request)) {
      if (db.find(request.key()) != db.end()) {
        GetReply reply;
        reply.set_value(db[request.key()]);
        stream->Write(reply);
      }
    }
    return Status::OK;
  }

  Status remove(ServerContext* context, const RemoveRequest* request, RemoveReply* response) override {
    auto pos = db.find(request->key());
    if (pos != db.end()){
      db.erase(pos);
    } 
    return Status::OK;
  }

 private:
  std::map<std::string, std::string> db;
};

void RunServer() {
  std::string server_address("0.0.0.0:50001");
  KeyValueStoreImpl service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}


int main(int argc, char** argv) {
  RunServer();
  return 0;
}