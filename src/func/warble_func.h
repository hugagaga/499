#ifndef WARBLE_FUNCTIONS_H_
#define WARBLE_FUNCTIONS_H_

#include <string>

#include "func.grpc.pb.h"
#include "warble.grpc.pb.h"
#include "warble.pb.h"
#include <google/protobuf/message.h>
#include "../kvstore/kvstore_client.h"

using google::protobuf::Message;
using google::protobuf::Any;

namespace warble {

struct key {
  std::string username;
  std::string type;
  std::string value;
};

class Functions {
 public:
  // Initialization
  void Init();
  // Convert struct key to string
  std::string structToString(key key);
  // Convert string to struct key
  key stringToStruct(std::string str);
  // Inserts a (Username, UserID) pair to the kvstore backend
  Any RegisterUser(Any);
  // Post a new warble(or reply), returns the id of the new warble
  Any Warble(Any);
  // Starts following a given user
  Any Follow(Any);
  // Reads a warble thread from the given id
  Any Read(Any);
  // Returns this user's following and followers
  Any Profile(Any);

 private:
  // warble id counter
  int currentWarbleid_;
};

}  // namespace warble

#endif  // WARBLE_FUNCTIONS_H_