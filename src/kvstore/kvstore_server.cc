#include "kvstore_server.h"

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpcpp/grpcpp.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "kvmap.h"
#include "kvstore.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using kvstore::GetReply;
using kvstore::GetRequest;
using kvstore::KeyValueStore;
using kvstore::PutReply;
using kvstore::PutRequest;
using kvstore::RemoveReply;
using kvstore::RemoveRequest;

DEFINE_string(store, "", "filename");

Status KeyValueStoreImpl::Put(ServerContext* context, const PutRequest* request,
                              PutReply* response) {
  const std::lock_guard<std::mutex> lock(mutex_);
  LOG(INFO) << "Put(" << request->key() << ", " << request->value() << ")" << std::endl;

  kvstore_.Put(request->key(), request->value());

  bool store = !FLAGS_store.empty();
  std::string filename = FLAGS_store;
  if (store) {
    kvstore_.Update(filename);
  }

  LOG(INFO) << "Status: OK " << std::endl;
  return Status::OK;
}

Status KeyValueStoreImpl::Get(
    ServerContext* context, ServerReaderWriter<GetReply, GetRequest>* stream) {
  GetRequest request;
  Status status = Status::OK;
  while (stream->Read(&request)) {
    auto strs = kvstore_.Get(request.key());

    GetReply reply;
    int size = 0;
    if (strs.has_value()) {
      auto values = *strs;
      LOG(INFO) << "Get values of key (" << request.key()
                << ") : " << std::endl;
      size = values.size();
      reply.set_value(std::to_string(size));
      stream->Write(reply);
      for (const std::string& str : values) {
        LOG(INFO) << str << " ";
        reply.set_value(str);
        stream->Write(reply);
      }
      LOG(INFO) << std::endl;
    } else {
      reply.set_value(std::to_string(size));
      stream->Write(reply);
      LOG(ERROR) << "Key is not found!" << std::endl;
      grpc::string error_string("The key is not found!");
      status = Status(grpc::StatusCode::NOT_FOUND, error_string);
    }
  }
  return status;
}

Status KeyValueStoreImpl::Remove(ServerContext* context,
                                 const RemoveRequest* request,
                                 RemoveReply* response) {
  Status status = Status::OK;
  bool success = kvstore_.Remove(request->key());
  if (!success) {
    grpc::string error_string("The key is not found!");
    status = Status(grpc::StatusCode::NOT_FOUND, error_string);
    LOG(ERROR) << "Key is not found!" << std::endl;
  } else {
    LOG(INFO) << "Remove values of key (" << request->key() << ")" << std::endl;
  }
  return status;
}

void KeyValueStoreImpl::Restore(std::string filename) {
  kvstore_.Restore(filename);
}

// start the server
void RunServer() {
  // server running on port 50001
  std::string server_address("0.0.0.0:50001");
  KeyValueStoreImpl service;
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());

  bool store = !FLAGS_store.empty();
  std::string filename = FLAGS_store;

  if (store) {
    service.Restore(filename);
  }

  std::cout << "Server listening on " << server_address << std::endl;
  LOG(INFO) << "Server listening on " << server_address << std::endl;

  server->Wait();
}

int main(int argc, char** argv) {
  // start logging
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);
  LOG(INFO) << "Run Server" << std::endl;
  RunServer();
  return 0;
}