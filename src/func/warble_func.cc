#include "warble_func.h"

#include <algorithm>
#include <chrono>
#include <optional>
#include <random>
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

std::string random_id(size_t length) {
  std::vector<char> ch_set({'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K',
                            'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U',
                            'V', 'W', 'X', 'Y', 'Z',
                            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
                            'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u',
                            'v', 'w', 'x', 'y', 'z'});
  std::string str(length, 0);
  std::default_random_engine rng(std::random_device{}());
  std::uniform_int_distribution<> dist(0, ch_set.size() - 1);
  auto rand_ch = [ch_set, &dist, &rng]() { return ch_set[dist(rng)]; };
  std::generate_n(str.begin(), length, rand_ch);
  return str;
}

bool hasUser(std::string username, StorageAbstraction &func) {
  Key keyMessage;
  keyMessage.set_username(username);
  keyMessage.set_type("username");
  std::string key;
  keyMessage.SerializeToString(&key);
  std::vector<std::string> temp;
  Status hasKey = func.GetOne(key, temp);
  return hasKey.ok();
}

// Helper function: To extract hashtags from the warble test
std::vector<std::string> extractHashTags(const std::string s) {
  if (s.empty()) {
    return {};
  }

  std::vector<std::string> res;
  int start = 0, end = 0;

  // start should not be the last char.
  while (start < s.size() - 1) {
    // find #
    while (start < s.size() - 1 && s[start] != '#') {
      start++;
    }

    if (start < s.size() - 1) {
      end = start + 1;

      // case like ######
      if (s[end] == '#') {
        start = end + 1;
        continue;
      }

      // find space or end
      while (end < s.size() && !isspace(s[end])) {
        end++;
      }
      std::string tag = s.substr(start + 1, end - 1 - start);
      res.push_back(tag);
      start = end + 1;
    }
  }

  return res;
}

// Helper function: To extract all the warbles which posted after the specified
// timestamp
std::vector<Warble> extractWarbles(std::vector<Warble> &warbles,
                                   const Timestamp &timestamp) {

  sort(warbles.begin(), warbles.end(), [](Warble comp1, Warble comp2) {
    return comp1.timestamp().useconds() < comp2.timestamp().useconds();
  });

  auto iter = warbles.begin();
  for (; iter != warbles.end(); iter++) {
    if (iter->timestamp().seconds() > timestamp.seconds()) {
      break;
    } else if (iter->timestamp().seconds() == timestamp.seconds()) {
      if (iter->timestamp().useconds() > timestamp.useconds()) {
        break;
      }
    }
  }

  return std::vector<Warble>(iter, warbles.end());
}

std::optional<Any> RegisterUser(Any input, StorageAbstraction &func) {
  std::optional<Any> ret = std::nullopt;
  // Parse input from Any to request
  RegisteruserRequest request;
  input.UnpackTo(&request);
  std::string username = request.username();

  if (!hasUser(username, func)) {
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

std::optional<Any> WarbleFunc(Any input, StorageAbstraction &func) {
  std::optional<Any> ret = std::nullopt;
  // Parse input from Any to request
  Key keyMessage;
  warble::Warble valueMessage;
  WarbleRequest request;
  input.UnpackTo(&request);
  std::string username = request.username();

  if (hasUser(username, func)) {
    std::string text = request.text();

    // Extract hashtags
    std::vector<std::string> hashtagList = extractHashTags(text);

    bool isReply = !request.parent_id().empty();
    // Generate a random warble id.
    std::string id = random_id(16);

    // If the warble is a reply, add it to the parent thread.
    if (isReply) {
      Key keyMessage;
      keyMessage.set_id(request.parent_id());
      keyMessage.set_type("warble");
      std::string key;
      keyMessage.SerializeToString(&key);
      std::vector<std::string> warbles;
      bool hasThread = func.GetOne(key, warbles).ok();
      if (hasThread) {
        // Create a new thread for current warble
        keyMessage.set_type("warble");
        keyMessage.set_id(id);
        valueMessage.set_parent_id(id);
        valueMessage.set_username(username);
        valueMessage.set_id(id);
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

        keyMessage.set_id(request.parent_id());
        valueMessage.set_parent_id(request.parent_id());
        std::string parentID;
        keyMessage.SerializeToString(&parentID);
        valueMessage.SerializeToString(&warble);

        func.Put(parentID, warble);

        // Store the hashtag entry into the kvstore
        for (const std::string &hashtag : hashtagList) {
          Key hashtagKeyMessage;
          hashtagKeyMessage.set_type("hashtag");
          hashtagKeyMessage.set_id(hashtag);

          std::string hashtagKey;
          hashtagKeyMessage.SerializeToString(&hashtagKey);

          func.Put(hashtagKey, warble);
        }

        warble::Warble* returned_warble = new warble::Warble(valueMessage);
        WarbleReply reply;
        reply.set_allocated_warble(returned_warble);
        Any output;
        output.PackFrom(reply);
        ret = std::optional<Any>{output};
      }
    } else {
      // Create a new thread for current warble
      keyMessage.set_type("warble");
      keyMessage.set_id(id);
      valueMessage.set_parent_id(id);
      valueMessage.set_username(username);
      valueMessage.set_id(id);
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

      // Store the hashtag entry into the kvstore
      for (const std::string &hashtag : hashtagList) {
        Key hashtagKeyMessage;
        hashtagKeyMessage.set_type("hashtag");
        hashtagKeyMessage.set_id(hashtag);

        std::string hashtagKey;
        hashtagKeyMessage.SerializeToString(&hashtagKey);

        func.Put(hashtagKey, warble);
      }

      warble::Warble* returned_warble = new warble::Warble(valueMessage);
      WarbleReply reply;
      reply.set_allocated_warble(returned_warble);
      Any output;
      output.PackFrom(reply);
      ret = std::optional<Any>{output};
    }
  }
  return ret;
}

std::optional<Any> Follow(Any input, StorageAbstraction &func) {
  std::optional<Any> ret = std::nullopt;
  // Parse input from Any to request message
  FollowRequest request;
  input.UnpackTo(&request);
  std::string username = request.username();
  std::string to_follow = request.to_follow();

  // Check if the user and the to-follow user exist
  if (hasUser(username, func) && hasUser(to_follow, func)) {
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

std::optional<Any> Read(Any input, StorageAbstraction &func) {
  std::optional<Any> ret = std::nullopt;
  // Parse input from Any to request message
  ReadRequest request;
  input.UnpackTo(&request);
  std::string warble_id = request.warble_id();
  std::string user = request.user();
  // Check if the user has registered
  if (hasUser(user, func)) {
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

std::optional<Any> Profile(Any input, StorageAbstraction &func) {
  std::optional<Any> ret = std::nullopt;
  // Parse input from Any to request message
  ProfileRequest request;
  input.UnpackTo(&request);
  std::string username = request.username();
  // Check if the user has registered
  if (hasUser(username, func)) {
    Key keyMessage;
    std::string key;
    ProfileReply reply;

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
    ret = std::optional<Any>{output};
  }
  return ret;
}

std::optional<Any> WarbleStream(Any input, StorageAbstraction &func) {
  std::optional<Any> ret = std::nullopt;
  // Parse input from Any to request message
  StreamRequest request;
  input.UnpackTo(&request);

  std::string hashtag = request.hashtag();
  Timestamp timestamp = request.timestamp();

  Key keyMessage;
  std::string key;
  StreamReply reply;

  keyMessage.set_type("hashtag");
  keyMessage.set_id(hashtag);
  keyMessage.SerializeToString(&key);

  std::vector<std::string> warblesAsStrings;
  bool hasWarbles = func.GetOne(key, warblesAsStrings).ok();

  // Deserialize warblesAsStrings to list of warbles.
  std::vector<Warble> warbles;
  if (hasWarbles) {
    for (const std::string &warbleAsString : warblesAsStrings) {
      Warble warble;
      warble.ParseFromString(warbleAsString);
      warbles.push_back(warble);
    }
  }

  std::vector<Warble> retWarbles = extractWarbles(warbles, timestamp);

  for (const Warble &warble : retWarbles) {
    auto warble_to_add = reply.add_warbles();
    warble_to_add->CopyFrom(warble);
  }

  Any output;
  output.PackFrom(reply);
  ret = std::optional<Any>{output};
  return ret;
}
} // namespace warble