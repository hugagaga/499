#include "func_infra.h"

#include <functional>
#include <unordered_set>

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
  client_.Init();
  std::function registeruser = [&](Any input) -> Any { return client_.RegisterUser(input);};
  map_.insert(std::make_pair(EventType::REGISTER_USER, registeruser));
  std::function warble = [&](Any input) -> Any { return client_.Warble(input);};
  map_.insert(std::make_pair(EventType::WARBLE, warble));
}

void FuncInfra::Hook(const EventType& e) { registeredList_.insert(e); }

void FuncInfra::Unhook(const EventType& e) { registeredList_.erase(e); }

bool FuncInfra::IsHooked(const EventType& e) {
  return registeredList_.find(e) != registeredList_.end();
}

void FuncInfra::EventHandler(const EventType& e, Any input, Any& output) {
  if (IsHooked(e)) {
    std::function<Any(Any)> callback = map_[e]; 
    output = callback(input);
  }
}

}  // namespace func