#ifndef WARBLE_KVSTORE_SERVER_H_
#define WARBLE_KVSTORE_SERVER_H_

#include <memory>
#include <mutex>
#include <string>


#include <grpcpp/grpcpp.h>

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

// KeyValueStore service implimentations
class KeyValueStoreImpl final : public KeyValueStore::Service {
 public:
  // Call Kvmap::Put(key, value) function
  Status Put(ServerContext* context, const PutRequest* request,
             PutReply* response) override;
  // Call Kvmap::Get(key) function
  Status Get(ServerContext* context,
             ServerReaderWriter<GetReply, GetRequest>* stream) override;
  // Call Kvmap::Remove(key) function
  Status Remove(ServerContext* context, const RemoveRequest* request,
                RemoveReply* response) override;
  // Restore data from previously-stored file
  void Restore(std::string filename);
 private:
  // Kvmap objects that contains kvstore functions
  kvstore::Kvmap kvstore_;
  // Mutex lock to keep Put function thread-safe
  std::mutex mutex_;
};

// Run the server, 
// If FLAGS_store is not empty, put results into the file of with given filename, FLAGS_store.
void RunServer();

#endif  // WARBLE_KVSTORE_SERVER_H_