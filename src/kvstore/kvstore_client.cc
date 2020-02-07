#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include <grpcpp/grpcpp.h>
//#include <glog/logging.h>

#include "kvstore.grpc.pb.h"
#include "kvstore_client.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;
using kvstore::KeyValueStore;
using kvstore::PutRequest;
using kvstore::PutReply;
using kvstore::GetRequest;
using kvstore::GetReply;
using kvstore::RemoveRequest;
using kvstore::RemoveReply;

KeyValueStoreClient::KeyValueStoreClient(std::shared_ptr<Channel> channel) : stub_(KeyValueStore::NewStub(channel)) {}
  
Status KeyValueStoreClient::Put(const std::string& key, const std::string& value) {
  //LOG(INFO) << "Start to Put \n";
  std::cout << "Run Put \n";
  PutRequest request;
  request.set_key(key);
  request.set_value(value);

  PutReply reply;
  ClientContext context;
  Status status = stub_->Put(&context, request, &reply);
  
  if (status.ok()) {
    std::cout << "Put(" << key << ", " << value << ") status: OK " << std::endl;
  } else {
    std::cout << status.error_code() << ": " << status.error_message() << std::endl;
  }
  return status;
}

Status KeyValueStoreClient::GetOne(const std::string& key, std::vector<std::string>& values) {
  std::cout << "Run GetOne \n";
  ClientContext context;
  Status status = Status::OK;
  auto stream = stub_->Get(&context);
  GetRequest request;
  request.set_key(key);
  stream->Write(request);

  GetReply reply;
  while (stream->Read(&reply)) {
    values.push_back(reply.value());
  }

  stream->WritesDone();
  status = stream->Finish();

  if (status.ok()) {
    std::cout << "Get values of key (" << key << ") : " << std::endl;
    for (int i = 0; i < values.size(); ++i) {
      std::cout << values[i] << " ";
    }
    std::cout << std::endl;
  }
  else {
    std::cout << status.error_code() << ": " << status.error_message() << std::endl;
  }
}

std::vector<Status> KeyValueStoreClient::Get(const std::vector<std::string>& keys, std::vector<std::vector<std::string>>& values) {
  std::cout << "Run Get \n";
  ClientContext context;
  std::vector<Status> status;
  for (const std::string& key : keys) {
    std::vector<std::string> v;
    status.push_back(GetOne(key,v));
  }
  return status;
}

Status KeyValueStoreClient::Remove(const std::string& key) {
  std::cout << "Run Remove \n";
  RemoveRequest request;
  request.set_key(key);

  RemoveReply reply;
  ClientContext context;
  Status status = stub_->Remove(&context, request, &reply);
  if (status.ok()) {
    std::cout << "Remove values of key (" << key << ") Status : OK " << std::endl;
  }
  else {
    std::cout << status.error_code() << ": " << status.error_message() << std::endl;
  }
  return status;
}

int main(int argc, char** argv) {
  //start logging
  //google::InitGoogleLogging(argv[0]);
  //Test KVstore function on GRPC
  KeyValueStoreClient func(grpc::CreateChannel("localhost:50001", grpc::InsecureChannelCredentials()));
  
  func.Put("apple", "red");
  func.Put("apple", "round");
  func.Put("banana", "yellow");
  //LOG(INFO) << "Start to Get \n";
  std::vector<std::string> keys {"apple", "banana"};
  std::vector<std::vector<std::string>> values;
  func.Get(keys, values);
  //LOG(INFO) << "Start to Remove \n";
  func.Remove("apple");
  std::string key {"apple"};
  std::vector<std::string> v;
  func.GetOne(key, v);
}