#include "kvstore_client.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <glog/logging.h>
#include <grpcpp/grpcpp.h>
#include "kvstore.grpc.pb.h"


using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using grpc::Status;
using kvstore::GetReply;
using kvstore::GetRequest;
using kvstore::KeyValueStore;
using kvstore::PutReply;
using kvstore::PutRequest;
using kvstore::RemoveReply;
using kvstore::RemoveRequest;

KeyValueStoreClient::KeyValueStoreClient(std::shared_ptr<Channel> channel)
    : stub_(KeyValueStore::NewStub(channel)) {}

Status KeyValueStoreClient::Put(const std::string& key,
                                const std::string& value) {
  PutRequest request;
  request.set_key(key);
  request.set_value(value);

  PutReply reply;
  ClientContext context;
  Status status = stub_->Put(&context, request, &reply);

  if (status.ok()) {
    LOG(INFO) << "Put(" << key << ", " << value << ") status: OK " << std::endl;
  } else {
    LOG(ERROR) << status.error_code() << ": " << status.error_message()
               << std::endl;
  }
  return status;
}

Status KeyValueStoreClient::GetOne(const std::string& key,
                                   std::vector<std::string>& values) {
  ClientContext context;
  Status status = Status::OK;
  auto stream = stub_->Get(&context);
  GetRequest request;
  request.set_key(key);
  stream->Write(request);

  GetReply reply;
  if (stream->Read(&reply)) {
    LOG(INFO) << "The number of values: " << reply.value() << std::endl;
    int size = std::stoi(reply.value(), nullptr, 10);

    for (int i = 0; i < size; ++i) {
      stream->Read(&reply);
      values.push_back(reply.value());
    }
  }
  stream->WritesDone();
  status = stream->Finish();

  if (status.ok()) {
    LOG(INFO) << "Get values of key (" << key << ") : " << std::endl;
    for (int i = 0; i < values.size(); ++i) {
      LOG(INFO) << values[i] << " ";
    }
    LOG(INFO) << std::endl;
  } else {
    LOG(ERROR) << status.error_code() << " : " << status.error_message()
               << std::endl;
  }
  return status;
}

std::vector<Status> KeyValueStoreClient::Get(
    const std::vector<std::string>& keys,
    std::vector<std::vector<std::string>>& values) {
  ClientContext context;
  std::vector<Status> status;
  for (const std::string& key : keys) {
    std::vector<std::string> v;
    status.push_back(GetOne(key, v));
    values.push_back(v);
  }
  return status;
}

Status KeyValueStoreClient::Remove(const std::string& key) {
  RemoveRequest request;
  request.set_key(key);

  RemoveReply reply;
  ClientContext context;
  Status status = stub_->Remove(&context, request, &reply);
  if (status.ok()) {
    LOG(INFO) << "Remove values of key (" << key << ") Status : OK "
              << std::endl;
  } else {
    LOG(ERROR) << status.error_code() << ": " << status.error_message()
               << std::endl;
  }
  return status;
}

/* For testing
int main(int argc, char** argv) {
  // start logging
  google::InitGoogleLogging(argv[0]);
  // Test KVstore function on GRPC
  KeyValueStoreClient func(grpc::CreateChannel(
      "localhost:50001", grpc::InsecureChannelCredentials()));

  func.Put("apple", "red");
  func.Put("apple", "round");
  func.Put("banana", "yellow");
  std::vector<std::string> keys{"apple", "banana"};
  std::vector<std::vector<std::string>> values;
  func.Get(keys, values);
  func.Remove("apple");
  std::string key{"apple"};
  std::vector<std::string> v;
  func.GetOne(key, v);
}
*/
