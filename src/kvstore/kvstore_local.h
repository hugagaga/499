#ifndef WARBLE_KVSTORE_LOCAL_H_
#define WARBLE_KVSTORE_LOCAL_H_

#include "storage_abstraction.h"

#include "kvmap.h"

// Local in-memory storage, which would be used for unit tests
// It has the same abstract class as KeyValueStoreClient class
class KeyValueStoreLocal : public StorageAbstraction {
public:
  // Calls function in the server to store key-value
  Status Put(const std::string &key, const std::string &value) override;
  // Get a value or values to one key
  // Returns Status::NOT_FOUND is the key does not exits, otherwise return
  // Status::OK
  Status GetOne(const std::string &key,
                std::vector<std::string> &values) override;
  // Call GetOne in a loop
  std::vector<Status>
  Get(const std::vector<std::string> &keys,
      std::vector<std::vector<std::string>> &values) override;
  // Remove values associated to the key in the server
  Status Remove(const std::string &key) override;
private:
  // Kvmap objects that contains kvstore functions
  kvstore::Kvmap kvstore_;
};

#endif // WARBLE_KVSTORE_LOCAL_H_
