#ifndef WARBLE_KVSTORE_CLIENT_H_
#define WARBLE_KVSTORE_CLIENT_H_

#include <memory>
#include <vector>
#include <string>

#include <grpcpp/grpcpp.h>

#include "kvstore.grpc.pb.h"

using grpc::Channel;
using grpc::Status;
using kvstore::KeyValueStore;


//Client class for clients to use key-value store service on the server 
class KeyValueStoreClient {
 public:
  KeyValueStoreClient(std::shared_ptr<Channel> channel);
  //Calls function in the server to store key-value
  Status Put(const std::string& key, const std::string& value);
  //Get a value or values to one key 
  //Returns Status::NOT_FOUND is the key does not exits, otherwise return Status::OK
  Status GetOne(const std::string& key, std::vector<std::string>& values);
  //Call GetOne in a loop
  std::vector<Status> Get(const std::vector<std::string>& keys, std::vector<std::vector<std::string>>& values);
  //Remove values associated to the key in the server
  Status Remove(const std::string& key);

 private:
  //KeyValueStore client stub to start RPC call.
  std::unique_ptr<KeyValueStore::Stub> stub_;
};

#endif  // WARBLE_KVSTORE_CLIENT_H_