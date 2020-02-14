#include "funcinfra.h"
#include "functions.h"

#include <any>
#include <unordered_set>

void FuncInfra::Hook(int eventType) { list_.insert(eventType); }

void FuncInfra::Unhook(int eventTyoe) { list_.erase(eventTyoe); }

bool FuncInfra::IsHooked(int eventType) {
  return list_.find(eventType) != list_.end();
}

void FuncInfra::EventHandler(int eventType, std::any input) {
  if (IsHooked(eventType)) {
    switch (eventType) {
      case REGISTER_USER:
        warble::Functions::RegisterUser(input);
        break;
      case WARBLE:
        warble::Functions::Warble(input);
        break;
      case FOLLOW:
        warble::Functions::Follow(input);
        break;
      case READ:
        warble::Functions::Read(input);
        break;
      case PROFILE:
        warble::Functions::Profile(input);
        break;
      default:
    }
  }
}