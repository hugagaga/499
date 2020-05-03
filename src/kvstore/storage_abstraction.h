#ifndef WARBLE__STORAGE_ABSTRACTION_H_
#define WARBLE__STORAGE_ABSTRACTION_H_
#include <memory>
#include <string>
#include <vector>

#include <grpcpp/grpcpp.h>

#include "kvstore.grpc.pb.h"

using grpc::Channel;
using grpc::Status;
using kvstore::KeyValueStore;

// A key-value storage abstraction that can enable storage and retrieval of
// data. The callers do not know the implementation of storage.
class StorageAbstraction {
 public:
  StorageAbstraction() = default;
  virtual ~StorageAbstraction() = default;

  // Calls function in the server to store key-value
  virtual Status Put(const std::string &key, const std::string &value) = 0;
  // Get a value or values to one key
  // Returns Status::NOT_FOUND is the key does not exits, otherwise return
  // Status::OK
  virtual Status GetOne(const std::string &key,
                        std::vector<std::string> &values) = 0;
  // Call GetOne in a loop
  virtual std::vector<Status>
  Get(const std::vector<std::string> &keys,
      std::vector<std::vector<std::string>> &values) = 0;
  // Remove values associated to the key in the server
  virtual Status Remove(const std::string &key) = 0;
};
#endif // WARBLE_E_STORAGE_ABSTRACTION_H_
