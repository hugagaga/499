#ifndef WARBLE_WARBLE_H_
#define WARBLE_WARBLE_H_

#include <string>

namespace warble {

struct warbleInput {
  std::string userName;
  std::string text;
  int warbleId;
};

class Functions {
 public:
  // Inserts a (Username, UserID) pair to the kvstore backend
  bool RegisterUser(std::string username);
  // Post a new warble(or reply), returns the id of the new warble
  int Warble(const warbleInput& input);
  // Starts following a given user
  void Follow(const std::pair<std::string, std::string>& input);
  // Reads a warble thread from the given id
  void Read(int id);
  // Returns this user's following and followers
  void Profile();

 private:
  // Current User ID, which increments when a new User registers
  int currentUserId_;
};

}  // namespace warble

#endif  // WARBLE_FUNCTIONS_H_