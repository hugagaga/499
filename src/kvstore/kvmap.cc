#include "kvmap.h"

#include <string>
#include <map>
#include <vector>

Kvmap::Kvmap() = default;

Kvmap::Kvmap(const std::vector<std::pair<std::string, std::string>>& list) {
  for (const auto& p : list) {
    kvmap_.insert(p);
  }
}

bool Kvmap::Put(const std::string& key, const std::string& value) {
  const auto [it, success] = kvmap_.insert({key, value});
  return success;
}

std::pair<std::string, int> Kvmap::Get(const std::string& key) {
  bool found = kvmap_.find(key) != kvmap_.end();
  if (!found) {
    return {"", -1};
  }
  return {kvmap_[key], 0};
}

bool Kvmap::Remove(const std::string& key) {
  int success = kvmap_.erase(key);
  return success;
}