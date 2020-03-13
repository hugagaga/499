#include "warble_func.h"

#include <chrono>
#include <optional>
#include <stack>
#include <string>
#include <vector>

#include "../kvstore/kvstore_client.h"
#include "func.grpc.pb.h"
#include "warble.grpc.pb.h"
#include "warble.pb.h"

using google::protobuf::Any;
using grpc::Status;
using kvstore::KeyValueStore;
using kvstore::PutReply;
using kvstore::PutRequest;

namespace warble {
void Functions::Init() { currentWarbleid_ = 0; }

bool Functions::hasUser(std::string username) {
  KeyValueStoreClient func(grpc::CreateChannel(
      "localhost:50001", grpc::InsecureChannelCredentials()));
  Key keyMessage;
  keyMessage.set_username(username);
  keyMessage.set_type("username");
  std::string key;
  keyMessage.SerializeToString(&key);
  std::vector<std::string> temp;
  Status hasKey = func.GetOne(key, temp);
  return hasKey.ok();
}

std::optional<Any> Functions::RegisterUser(Any input) {
  std::optional<Any> ret = std::nullopt;
  // Parse input from Any to request
  RegisteruserRequest request;
  input.UnpackTo(&request);
  std::string username = request.username();

  if (!hasUser(username)) {
    KeyValueStoreClient func(grpc::CreateChannel(
        "localhost:50001", grpc::InsecureChannelCredentials()));
    Key keyMessage;
    keyMessage.set_username(username);
    keyMessage.set_type("username");
    std::string key;
    keyMessage.SerializeToString(&key);

    func.Put(key, username);

    RegisteruserReply reply;
    Any output;
    output.PackFrom(reply);
    ret = std::optional<Any>{output};
  }
  return ret;
}

std::optional<Any> Functions::Warble(Any input) {
  std::optional<Any> ret = std::nullopt;
  // Parse input from Any to request
  Key keyMessage;
  warble::Warble valueMessage;
  WarbleRequest request;
  input.UnpackTo(&request);
  std::string username = request.username();

  if (hasUser(username)) {
    std::string text = request.text();
    bool isReply = !request.parent_id().empty();
    KeyValueStoreClient func(grpc::CreateChannel(
        "localhost:50001", grpc::InsecureChannelCredentials()));
    // Create a new thread for current warble
    keyMessage.set_type("warble");
    keyMessage.set_id(std::to_string(currentWarbleid_));
    valueMessage.set_parent_id(std::to_string(currentWarbleid_));
    valueMessage.set_username(username);
    valueMessage.set_id(std::to_string(currentWarbleid_));
    valueMessage.set_text(text);
    auto now = std::chrono::system_clock::now().time_since_epoch();
    Timestamp* time = new Timestamp();
    time->set_seconds(
        std::chrono::duration_cast<std::chrono::seconds>(now).count());
    time->set_useconds(
        std::chrono::duration_cast<std::chrono::milliseconds>(now).count());
    valueMessage.set_allocated_timestamp(time);
    std::string currentID;
    std::string warble;
    keyMessage.SerializeToString(&currentID);
    valueMessage.SerializeToString(&warble);

    func.Put(currentID, warble);

    // If the warble is a reply, add it to the parent thread.
    if (isReply) {
      keyMessage.set_id(request.parent_id());
      valueMessage.set_parent_id(request.parent_id());
      std::string parentID;
      keyMessage.SerializeToString(&parentID);
      valueMessage.SerializeToString(&warble);

      func.Put(parentID, warble);
    }
    warble::Warble* returned_warble = new warble::Warble(valueMessage);
    WarbleReply reply;
    reply.set_allocated_warble(returned_warble);
    Any output;
    output.PackFrom(reply);
    ret = std::optional<Any>{output};
    currentWarbleid_++;
  }
  return ret;
}

std::optional<Any> Functions::Follow(Any input) {
  std::optional<Any> ret = std::nullopt;
  // Parse input from Any to request message
  FollowRequest request;
  input.UnpackTo(&request);
  std::string username = request.username();
  std::string to_follow = request.to_follow();

  // Check if the user and the to-follow user exist
  if (hasUser(username) && hasUser(to_follow)) {
    KeyValueStoreClient func(grpc::CreateChannel(
        "localhost:50001", grpc::InsecureChannelCredentials()));
    Key keyMessage;
    std::string followers;
    std::string following;

    keyMessage.set_type("followers");
    keyMessage.set_username(to_follow);
    keyMessage.SerializeToString(&followers);
    keyMessage.set_type("following");
    keyMessage.set_username(username);
    keyMessage.SerializeToString(&following);

    func.Put(followers, username);
    func.Put(following, to_follow);

    FollowReply reply;
    Any output;
    output.PackFrom(reply);
    ret = std::optional<Any>{output};
  }
  return ret;
}

std::optional<Any> Functions::Read(Any input) {
  std::optional<Any> ret = std::nullopt;
  // Parse input from Any to request message
  ReadRequest request;
  input.UnpackTo(&request);
  std::string warble_id = request.warble_id();
  std::string user = request.user();
  // Check if the user has registered
  if (hasUser(user)) {
    KeyValueStoreClient func(grpc::CreateChannel(
        "localhost:50001", grpc::InsecureChannelCredentials()));
    Key keyMessage;
    keyMessage.set_id(warble_id);
    keyMessage.set_type("warble");

    // Iterative DFS for all warbles in a thread
    std::string key;
    keyMessage.SerializeToString(&key);
    std::vector<std::string> warbles;
    bool hasThread = func.GetOne(key, warbles).ok();
    if (hasThread) {
      ReadReply reply;
      int j = 0;
      warble::Warble s;
      s.ParseFromString(warbles[j]);
      while (s.id() != warble_id) {
        j++;
        s.ParseFromString(warbles[j]);
      }
      std::stack<warble::Warble> stack;
      stack.push(s);
      while (!stack.empty()) {
        warble::Warble* warble_to_add = reply.add_warbles();
        warble::Warble w = stack.top();
        *warble_to_add = w;
        stack.pop();

        keyMessage.set_id(warble_to_add->id());
        keyMessage.set_type("warble");
        keyMessage.SerializeToString(&key);
        std::vector<std::string> ws;
        hasThread = func.GetOne(key, ws).ok();
        if (hasThread) {
          for (int i = 0; i < ws.size(); ++i) {
            warble::Warble warble;
            warble.ParseFromString(ws[i]);
            if (warble_to_add->id() != warble.id()) {
              stack.push(warble);
            }
          }
        }
      }
      Any output;
      output.PackFrom(reply);
      ret = std::optional<Any>{output};
    }
  }
  return ret;
}

std::optional<Any> Functions::Profile(Any input) {
  std::optional<Any> ret = std::nullopt;
  // Parse input from Any to request message
  ProfileRequest request;
  input.UnpackTo(&request);
  std::string username = request.username();
  // Check if the user has registered
  if (hasUser(username)) {
    Key keyMessage;
    std::string key;
    ProfileReply reply;

    KeyValueStoreClient func(grpc::CreateChannel(
        "localhost:50001", grpc::InsecureChannelCredentials()));
    keyMessage.set_type("followers");
    keyMessage.set_username(username);
    keyMessage.SerializeToString(&key);
    std::vector<std::string> followers;
    bool hasFollowers = func.GetOne(key, followers).ok();

    if (hasFollowers) {
      for (int i = 0; i < followers.size(); ++i) {
        reply.add_followers(followers[i]);
      }
    }
    
    keyMessage.set_type("following");
    keyMessage.SerializeToString(&key);
    std::vector<std::string> following;
    bool hasFollowing = func.GetOne(key, following).ok();

    if (hasFollowing) {
      for (int i = 0; i < following.size(); ++i) {
        reply.add_following(following[i]);
      }
    }
    Any output;
    output.PackFrom(reply);
    ret = std::optional<Any> {output};
  }
  return ret;
}

}  // namespace warble