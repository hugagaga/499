#ifndef WARBLE_KVMAP_H_
#define WARBLE_KVMAP_H_

#include <vector>
#include <string>
#include <unordered_map>
#include <optional>


//A class for backend key-value store
class Kvmap {
 public:
  //Default Constructor
  Kvmap();
  //Constructor of the kvmap_ from a vector of pairs<string, string>
  Kvmap(const std::vector<std::pair<std::string, std::string>>&);
  //Inserts <string, string> pair into the map
  void Put(const std::string& key, const std::string& value);
  //Returns an optional contained value which is 
  //present when there exits previously stored value or values under that key 
  //not present when if the key is not reprent in the store
  //has_value could be called to check if the returned object contains a value
  std::optional<std::vector<std::string>> Get(const std::string& key);
  //deletes all previously stored values under that key
  int Remove(const std::string& key);

 private:
  std::unordered_multimap<std::string, std::string> kvmap_;
};

#endif  // WARBLE_KVSTORE_H_