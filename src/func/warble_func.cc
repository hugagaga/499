#include "warble_func.h"

#include <string>
#include <chrono>

#include "func.grpc.pb.h"
#include "warble.grpc.pb.h"
#include "warble.pb.h"
#include "../kvstore/kvstore_client.h"

using google::protobuf::Any;
using kvstore::KeyValueStore;
using kvstore::PutReply;
using kvstore::PutRequest;
using warble::RegisteruserReply;
using warble::RegisteruserRequest;
using warble::WarbleRequest;
using warble::WarbleReply;
using warble::Warble;
using warble::Timestamp;

namespace warble {
void Functions::Init() {
  currentWarbleid_ = 0;
}

std::string Functions::structToString(key key) {
  std::string str = key.username + "&" + key.type + "&" + key.value;
  return str;
}

key Functions::stringToStruct(std::string str) {
  key key;
  int pos1 = str.find('&', 0);
  key.username = str.substr(0, pos1);
  int pos2 = str.find('&', pos1 + 1);
  key.type = str.substr(pos1 + 1, pos2 - pos1 - 1);
  key.value = str.substr(pos2 + 1);
}

Any Functions::RegisterUser(Any input) {
  KeyValueStoreClient func(grpc::CreateChannel(
      "localhost:50001", grpc::InsecureChannelCredentials()));
  key struct_input;
  RegisteruserRequest request;
  input.UnpackTo(&request);
  std::string username = request.username();
  struct_input.username = username;
  struct_input.type = "username";
  std::string key = structToString(struct_input);
  std::string value = username;
  func.Put(key, value);
  RegisteruserReply reply;
  Any output;
  output.PackFrom(reply);
  return output;
}

Any Functions::Warble(Any input) {
  KeyValueStoreClient func(grpc::CreateChannel(
      "localhost:50001", grpc::InsecureChannelCredentials()));
  key struct_input;
  WarbleRequest request;
  input.UnpackTo(&request);
  std::string username = request.username();
  std::string text = request.text();
  struct_input.username = username;
  struct_input.type = warble;
  struct_input.value = currentWarbleid_;
  std::string key = structToString(struct_input);
  std::string value = text;
  func.Put(key, value);
  Warble output;
  output.set_username = username;
  output.set_text = text;
  output.set_id = currentWarbleid_;
  auto now = std::chrono::system_clock::now().time_since_epoch();
  Timestamp time;
  time.set_seconds = duration_cast<std::chrono::seconds>(now);
  time.set_useconds = duration_cast<std::chrono::milliseconds>(now);
  output.set_allocated_timestamp(&time);
  WarbleReply reply;
  reply.set_allocated_warble(&warble);
  currentWarbleid_++;
}

}  // namespace warble