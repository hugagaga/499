#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "kvstore.grpc.pb.h"

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

class KeyValueStoreClient {
 public:
  KeyValueStoreClient(std::shared_ptr<Channel> channel) : stub_(KeyValueStore::NewStub(channel)) {}

  Status Put(const std::string& key, const std::string& value) {
    PutRequest request;
    request.set_key(key);
    request.set_value(value);

    PutReply reply;
    ClientContext context;
    Status status = stub_->put(&context, request, &reply);
    if (!status.ok()) {
      //TODO: use glog
      std::cout << status.error_code() << ": " << status.error_message() << std::endl;
    }
    return status;
  }

  std::string Get(const std::vector<std::string>& keys) {
    ClientContext context;
    auto stream = stub_->get(&context);
    for (const std::string& key : keys) {
      GetRequest request;
      request.set_key(key);
      stream->Write(request);

      GetReply reply;
      stream->Read(&reply);
      std::cout << key << " : " << reply.value() << std::endl;
    }
    stream->WritesDone();
    Status status = stream->Finish();
    if (status.ok()) {
      return "ok";
    } else {
      std::cout << status.error_code() << ": " << status.error_message() << std::endl;
      return "RPC failed";
    }
  }

  Status Remove(const std::string& key) {
    RemoveRequest request;
    request.set_key(key);

    RemoveReply reply;
    ClientContext context;
    Status status = stub_->remove(&context, request, &reply);
    if (!status.ok()) {
      //TODO: use glog
      std::cout << status.error_code() << ": " << status.error_message() << std::endl;
    }
    return status;
  }

    private:
     //KeyValueStore client stub to start RPC call.
     std::unique_ptr<KeyValueStore::Stub> stub_;
};

int main(int argc, char** argv) {
  KeyValueStoreClient func(grpc::CreateChannel("localhost:50001", grpc::InsecureChannelCredentials()));
  //TODO: testing code, use glog 
  Status replyput = func.Put("apple", "red");
  if (replyput.ok()) std::cout << "Put(apple, red) status: OK " << std::endl;
  replyput = func.Put("banana", "yellow");
  if (replyput.ok()) std::cout << "Put(banana, yellow) status: OK " << std::endl;
  std::vector<std::string> keys {"apple", "banana"};
  std::string replyget = func.Get(keys);
  std::cout << "Get() status: " << replyget << std::endl;
  Status replyrm = func.Remove("banana");
  if (replyrm.ok()) std::cout << "Remove status: OK " << std::endl;
  std::vector<std::string> key {"banana"};
  replyget = func.Get(keys);
}