#ifndef WARBLE_FUNCINFRA_H_
#define WARBLE_FUNCINFRA_H_

#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <mutex>

#include "func.grpc.pb.h"
#include <google/protobuf/message.h>
#include "warble_func.h"

using google::protobuf::Message;
using google::protobuf::Any;

namespace func {

// All event types the func layer deal with
enum class EventType { REGISTER_USER, WARBLE, FOLLOW, READ, PROFILE };

// Func Faas infrastructure
class FuncInfra {
 public:
  // Initialize the EventType to function map 
  void Init();
  // Hook a specific function for a input event type
  void Hook(const EventType& e);
  // Unhook a specific function for a input event type
  void Unhook(const EventType& e);
  // Helper function to check if a function is hooked
  bool IsHooked(const EventType& e);
  // Invoke a specific function to deal with a input event type
  void EventHandler(const EventType& e, Any input, std::optional<Any>& output);

 private:
  // A set of all registered event types
  std::unordered_set<EventType> registeredList_;
  // A map to map all event types to functions
  std::unordered_map<EventType, std::function<std::optional<Any> (Any)>> map_;
  // Make registeredList_ thread safe
  std::mutex mutex_;
};

}  // namespace func
#endif  // WARBLE_FUNCINFRA_H_