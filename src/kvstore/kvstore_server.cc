#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <glog/logging.h>

#include "kvmap.h"
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
using kvstore::RemoveRequest;
using kvstore::RemoveReply;

//KeyValueStore service implimentation
class KeyValueStoreImpl final : public KeyValueStore::Service {
 public:
  //Call Kvmap::Put(key, value) function
  Status Put(ServerContext* context, const PutRequest* request, PutReply* response) override {
    kvstore_.Put(request->key(), request->value());
    //TODO:handle ALREADY_EXISTS status
    return Status::OK;
  }
  
  //Call Kvmap::Get(key) function
  Status Get(ServerContext* context, ServerReaderWriter<GetReply, GetRequest>* stream) override {
    GetRequest request;
    std::vector<Status> status;
    while (stream->Read(&request)) {
      auto str = kvstore_.Get(request.key());
      
      GetReply reply;
      if (str) {
        reply.set_value(*str);
      } else {
        reply.set_value("");
        LOG(INFO) << "Key is not found!" << std::endl;
      }
      stream->Write(reply);
      //TODO:handle NOT_FOUND status
      status.push_back(Status::OK);
    }
    return status;
  }

  //Call Kvmap::Remove(key) function
  Status Remove(ServerContext* context, const RemoveRequest* request, RemoveReply* response) override {
    bool success = kvstore_.Remove(request->key());
    //TODO:handle NOT_FOUND status
    return Status::OK;
  }

 private:
  Kvmap kvstore_;
};

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
  server->Wait();
}


int main(int argc, char** argv) {
  //start logging
  google::InitGoogleLogging(argv[0]);
  RunServer();
  return 0;
}