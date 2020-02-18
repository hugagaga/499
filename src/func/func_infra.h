#ifndef WARBLE_FUNCINFRA_H_
#define WARBLE_FUNCINFRA_H_

#include <unordered_map>
#include <unordered_set>
#include <functional>

#include <google/protobuf/message.h>
#include "functions.h"

namespace func {

//The number of events
const int EventNum = 5;
// All event types the func layer deal with
enum EventType { REGISTER_USER, WARBLE, FOLLOW, READ, PROFILE };

// Func Faas infrastructure
class FuncInfra {
 public:
  // Hook a specific function for a input event type
  void Hook(EventType e);
  // Unhook a specific function for a input event type
  void Unhook(EventType e);
  // Helper function to check if a function is hooked
  bool IsHooked(EventType e);
  // Invoke a specific function to deal with a input event type
  // Returns the status of execution
  int EventHandler(EventType e, const google::protobuf::Message*);

 private:
  // A set of all registered event types
  std::unordered_set<EventType> list_;
  // A map to map all event types to functions
  std::unordered_map<EventType, std::function<void()>> map_;
};

}  // namespace func
#endif  // WARBLE_FUNCINFRA_H_