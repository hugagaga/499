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
  std::function registeruser = [&](Any input) -> std::optional<Any> { return warble::RegisterUser(input);};
  map_.insert(std::make_pair(EventType::REGISTER_USER, registeruser));
  std::function warble = [&](Any input) -> std::optional<Any> { return warble::WarbleFunc(input);};
  map_.insert(std::make_pair(EventType::WARBLE, warble));
  std::function follow = [&](Any input) -> std::optional<Any> { return warble::Follow(input);};
  map_.insert(std::make_pair(EventType::FOLLOW, follow));
  std::function read = [&](Any input) -> std::optional<Any> { return warble::Read(input);};
  map_.insert(std::make_pair(EventType::READ, read));
  std::function profile = [&](Any input) -> std::optional<Any> { return warble::Profile(input);};
  map_.insert(std::make_pair(EventType::PROFILE, profile));
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

void FuncInfra::EventHandler(const EventType& e, Any input, std::optional<Any>& output) {
  if (IsHooked(e)) {
    std::function<std::optional<Any>(Any)> callback = map_[e]; 
    output = callback(input);
  }
}

}  // namespace func