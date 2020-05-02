#include "func_infra.h"

#include <functional>
#include <unordered_set>
#include <optional>
#include <mutex>

#include <google/protobuf/message.h>
#include "warble_func.h"
#include "func.grpc.pb.h"
#include "warble.grpc.pb.h"
#include "warble.pb.h"

using google::protobuf::Message;
using google::protobuf::Any;
using warble::RegisteruserRequest;
using warble::RegisteruserReply;

namespace func {

void FuncInfra::Init() {
  map_ = {
      {EventType::REGISTER_USER,
       std::function<std::optional<Any>(Any, StorageAbstraction &)>(
           warble::RegisterUser)},
      {EventType::WARBLE,
       std::function<std::optional<Any>(Any, StorageAbstraction &)>(
           warble::WarbleFunc)},
      {EventType::FOLLOW,
       std::function<std::optional<Any>(Any, StorageAbstraction &)>(
           warble::Follow)},
      {EventType::READ,
       std::function<std::optional<Any>(Any, StorageAbstraction &)>(
           warble::Read)},
      {EventType::PROFILE,
       std::function<std::optional<Any>(Any, StorageAbstraction &)>(
           warble::Profile)},
      {EventType::STREAM,
       std::function<std::optional<Any>(Any, StorageAbstraction &)>(
           warble::WarbleStream)},
  };
}

void FuncInfra::Hook(const EventType& e) { 
  const std::lock_guard<std::mutex> lock(mutex_);
  registeredList_.insert(e); 
}

void FuncInfra::Unhook(const EventType& e) { 
  const std::lock_guard<std::mutex> lock(mutex_);
  registeredList_.erase(e); 
}

bool FuncInfra::IsHooked(const EventType& e) {
  const std::lock_guard<std::mutex> lock(mutex_);
  return (registeredList_.find(e) != registeredList_.end());
}

void FuncInfra::EventHandler(const EventType &e, Any input,
                             std::optional<Any> &output) {
  if (IsHooked(e)) {
    std::function<std::optional<Any>(Any, StorageAbstraction &)> callback =
        map_[e];
    KeyValueStoreClient func(grpc::CreateChannel(
        "localhost:50001", grpc::InsecureChannelCredentials()));
    output = callback(input, func);
  }
}

}  // namespace func