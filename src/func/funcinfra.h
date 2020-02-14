#ifndef WARBLE_FUNCINFRA_H_
#define WARBLE_FUNCINFRA_H_

#include <unordered_set>

#define REGISTER_USER 1
#define WARBLE 2
#define FOLLOW 3
#define READ 4
#define PROFILE 5
// Func Faas infrastructure
class FuncInfra {
 public:
  // Hook a specific function for a input event type
  void Hook(int eventType);
  // Unhook a specific function for a input event type
  void Unhook(int eventType);
  // Helper function to check if a function is hooked
  bool IsHooked(int eventType);
  // Invoke a specific function to deal with a input event type
  // Returns the status of execution
  int EventHandler(int eventType, std::any input);

 private:
  // A list to record registered functions
  std::unordered_set<int> list_;
}

#endif  // WARBLE_FUNCINFRA_H_