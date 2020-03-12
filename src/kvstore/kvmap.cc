#include "kvmap.h"

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>

namespace kvstore {
  
Kvmap::Kvmap() = default;

void Kvmap::KvmapSetup(
    const std::vector<std::pair<std::string, std::string>>& list) {
  for (const auto& p : list) {
    const std::lock_guard<std::mutex> lock(mutex_);
    kvmap_.insert(p);
  }
}

void Kvmap::Put(const std::string& key, const std::string& value) {
  const std::lock_guard<std::mutex> lock(mutex_);
  kvmap_.insert({key, value});
}

std::optional<std::vector<std::string>> Kvmap::Get(const std::string& key) {
  const std::lock_guard<std::mutex> lock(mutex_);
  auto range = kvmap_.equal_range(key);
  std::vector<std::string> values;
  std::optional<std::vector<std::string>> ret = std::nullopt;
  if (range.first != kvmap_.end()) {
    for (auto it = range.first; it != range.second; ++it) {
      values.push_back(it->second);
    }
    ret = std::optional<std::vector<std::string>>{values};
  }
  return ret;
}

int Kvmap::Remove(const std::string& key) {
  const std::lock_guard<std::mutex> lock(mutex_);
  int num = kvmap_.erase(key);
  return num;
}

}  // namespace kvstore