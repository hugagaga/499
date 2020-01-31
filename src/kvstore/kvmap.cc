#include "kvmap.h"

#include <string>
#include <map>
#include <vector>

Kvmap::Kvmap() = default;

Kvmap::Kvmap(std::vector<std::pair<std::string, std::string>> list) {
  for (const auto& p : list) {
    kvmap_.insert(p);
  }
}

bool Kvmap::put(const std::string& key, const std::string& value) {
  const auto [it, success] = kvmap_.insert({key, value});
  return success;
}

std::string Kvmap::get(const std::string& key) {
  bool found = kvmap_.find(key) != kvmap_.end();
  if (found) return kvmap_[key];
  return "NOT_FOUND";
}

bool Kvmap::remove(const std::string& key) {
  int success = kvmap_.erase(key);
  return success;
}