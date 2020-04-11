#ifndef WARBLE_KVMAP_H_
#define WARBLE_KVMAP_H_

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>

namespace kvstore{
// A class for backend key-value store
class Kvmap {
 public:
  // Default Constructor
  Kvmap();
  // Construct the kvmap_ from a vector of pairs<string, string> (for testing)
  void KvmapSetup(const std::vector<std::pair<std::string, std::string>>&);
  // Inserts <string, string> pair into the map
  void Put(const std::string& key, const std::string& value);
  // Returns an optional contained value which is
  // present when there exits previously stored value or values under that key
  // not present when if the key is not reprent in the store
  // has_value could be called to check if the returned object contains a value
  std::optional<std::vector<std::string>> Get(const std::string& key);
  // deletes all previously stored values under that key
  int Remove(const std::string& key);
  // update records of the file of given filename
  void Update(std::string filename);
  // restore data from the previous stored file
  void Restore(std::string filename);
  
 private:
  // A multimap as a database 
  std::unordered_multimap<std::string, std::string> kvmap_;
  // Make the kvmap_ thread safe
  std::mutex mutex_;
};
}  // namespace
#endif  // WARBLE_KVSTORE_H_