#ifndef WARBLE_KVMAP_H_
#define WARBLE_KVMAP_H_

#include <vector>
#include <string>
#include <map>


class Kvmap {
 public:
  Kvmap();
  Kvmap(std::vector<std::pair<std::string, std::string>>);
  bool put(const std::string& key, const std::string& value);
  std::string get(const std::string& key);
  bool remove(const std::string& key);
 private:
  std::map<std::string, std::string> kvmap_;
};

#endif  // WARBLE_KVSTORE_H_