#ifndef WARBLE_FUNCTIONS_H_
#define WARBLE_FUNCTIONS_H_

#include "kvstore_client.h"

namespace warble {

struct warbleInput {
  std::string userName;
  std::string text;
  int warbleId;
}

class Functions {
 public:
  // Inserts a (Username, UserID) pair to the kvstore backend
  void RegisterUser(std::string username);
  // Post a new warble(or reply), returns the id of the new warble
  int Warble(const warbleInput& input);
  // Starts following a given user
  void Follow(const std::pair<std::string, std::string>& input);
  // Reads a warble thread from the given id
  void Read(int id);
  // Returns this user's following and followers
  void Profile();

 private:
  int currentUserId_;
}
}  // namespace warble

#endif  // WARBLE_FUNCTIONS_H_