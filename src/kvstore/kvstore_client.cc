#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include <grpcpp/grpcpp.h>
#include <glog/logging.h>

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

//Get function return type
struct GetReturn {
  std::vector<std::string> values;
  Status status;
};

//Client class for clients to use key-value store service on the server 
class KeyValueStoreClient {
 public:
  KeyValueStoreClient(std::shared_ptr<Channel> channel) : stub_(KeyValueStore::NewStub(channel)) {}
  
  //Calls function in the server to store key-value
  Status Put(const std::string& key, const std::string& value) {
    PutRequest request;
    request.set_key(key);
    request.set_value(value);

    PutReply reply;
    ClientContext context;
    Status status = stub_->Put(&context, request, &reply);
    
    if (!status.ok()) {
      LOG(ERROR) << status.error_code() << ": " << status.error_message() << std::endl;
    }
    return status;
  }
  
  //Get the stream of values given a stream of keys from the server
  //Returns all the values and corresponding stauts in a struct GetReturn
  GetReturn Get(const std::vector<std::string>& keys) {
    ClientContext context;
    GetReturn res;
    auto stream = stub_->Get(&context);
    for (const std::string& key : keys) {
      GetRequest request;
      request.set_key(key);
      stream->Write(request);

      GetReply reply;
      stream->Read(&reply);
      res.values.push_back(reply.value());

      LOG(INFO) << key << " : " << reply.value() << std::endl;
    }
    stream->WritesDone();
    res.status = stream->Finish();

    if (!res.status.ok()) {
      LOG(ERROR) << res.status.error_code() << ": " << res.status.error_message() << std::endl;
    }
    return res;
  }
  
  //Remove values associated to the key in the server
  Status Remove(const std::string& key) {
    RemoveRequest request;
    request.set_key(key);

    RemoveReply reply;
    ClientContext context;
    Status status = stub_->Remove(&context, request, &reply);
    if (!status.ok()) {
      LOG(ERROR) << status.error_code() << ": " << status.error_message() << std::endl;
    }
    return status;
  }

 private:
  //KeyValueStore client stub to start RPC call.
  std::unique_ptr<KeyValueStore::Stub> stub_;
};

int main(int argc, char** argv) {
  //start logging
  google::InitGoogleLogging(argv[0]);
  //Test KVstore function on GRPC
  KeyValueStoreClient func(grpc::CreateChannel("localhost:50001", grpc::InsecureChannelCredentials()));
  
  Status replyput = func.Put("apple", "red");
  if (replyput.ok()) {
    LOG(INFO) << "Put(apple, red) status: OK " << std::endl;
  }
  replyput = func.Put("banana", "yellow");
  if (replyput.ok()) {
    LOG(INFO) << "Put(banana, yellow) status: OK " << std::endl;
  }
  std::vector<std::string> keys {"apple", "banana"};
  GetReturn replyget = func.Get(keys);
  if (replyget.status.ok()) {
    LOG(INFO) << "Get [apple, banana] status: OK " << std::endl;
  }
  Status replyrm = func.Remove("banana");
  if (replyrm.ok()) {
    LOG(INFO) << "Remove status: OK " << std::endl;
  }
  std::vector<std::string> key {"banana"};
  replyget = func.Get(keys);
  if (replyget.status.ok()) {
    LOG(INFO) << "Get [banana] status: OK " << std::endl;
  }
}