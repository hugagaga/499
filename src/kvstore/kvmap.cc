#include "kvmap.h"

#include <fstream>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "kvstore.grpc.pb.h"

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

void Kvmap::Update(const std::string& filename) {
  Kvpairs res;
  std::ofstream ofs;
  ofs.open(filename, std::ios_base::trunc);
  if (ofs.is_open()) {
    for (auto const& x : kvmap_) {
      PutRequest* p = res.add_pair();
      p->set_key(x.first);
      p->set_value(x.second);
    }
    std::string output;
    res.SerializeToString(&output);
    ofs << output;
    ofs.close();
  }
}

void Kvmap::Restore(const std::string& filename) {
  std::ifstream ifs;
  ifs.open(filename, std::ios_base::in);
  if (ifs.is_open()) {
    std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
    Kvpairs res;
    res.ParseFromString(content);
    for (int i = 0; i < res.pair_size(); ++i) {
      Put(res.pair(i).key(), res.pair(i).value());
    }
  }
}

}  // namespace kvstore