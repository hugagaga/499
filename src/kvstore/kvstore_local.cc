#include "kvstore_local.h"

Status KeyValueStoreLocal::Put(const std::string &key,
                               const std::string &value) {
  kvstore_.Put(key, value);
  return Status::OK;
}

Status KeyValueStoreLocal::GetOne(const std::string &key,
                                  std::vector<std::string> &values) {
  Status status = Status::OK;
  auto strs = kvstore_.Get(key);
  if (strs.has_value()) {
    auto strsValues = strs.value();
    for (const std::string &str : strsValues) {
      values.push_back(str);
    }
  } else {
    grpc::string error_string("The key is not found!");
    status = Status(grpc::StatusCode::NOT_FOUND, error_string);
  }
  return status;
}

std::vector<Status>
KeyValueStoreLocal::Get(const std::vector<std::string> &keys,
                        std::vector<std::vector<std::string>> &values) {
  std::vector<Status> status;
  for (const std::string &key : keys) {
    std::vector<std::string> v;
    status.push_back(GetOne(key, v));
    values.push_back(v);
  }
  return status;
}

Status KeyValueStoreLocal::Remove(const std::string &key) {
  Status status = Status::OK;
  bool success = kvstore_.Remove(key);
  if (!success) {
    grpc::string error_string("The key is not found!");
    status = Status(grpc::StatusCode::NOT_FOUND, error_string);
  }
  return status;
}
