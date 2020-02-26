#include "func_infra.h"

#include <unordered_set>

#include <google/protobuf/message.h>
#include "../warble/warble.h"

namespace func {

void FuncInfra::Hook(EventType e) { list_.insert(e); }

void FuncInfra::Unhook(EventType e) { list_.erase(e); }

bool FuncInfra::IsHooked(EventType e) {
  return list_.find(e) != list_.end();
}

int FuncInfra::EventHandler(EventType e, const google::protobuf::Message*) {
  if (IsHooked(e)) {
    EventType e = static_cast<EventType>(e);
    //TODO: Figure out how to use <EventType, function> map
  }
}

}  // namespace func