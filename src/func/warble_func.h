#ifndef WARBLE_FUNCTIONS_H_
#define WARBLE_FUNCTIONS_H_

#include <string>
#include <optional>

#include <google/protobuf/message.h>
#include "../kvstore/kvstore_client.h"
#include "func.grpc.pb.h"
#include "warble.grpc.pb.h"
#include "warble.pb.h"

using google::protobuf::Any;
using google::protobuf::Message;

namespace warble {
// Generate random id
std::string random_id(size_t length);
// protobuf message warble::Key {username, type, id}
// Inserts a Serialized (warble::Key, username) pair to the kvstore backend
std::optional<Any> RegisterUser(Any input, StorageAbstraction &);
// Post a new warble(or reply), returns the new warble in warble::Warble
std::optional<Any> WarbleFunc(Any input, StorageAbstraction &);
// Starts following a given user,
// insert ({target_username, type = "followers"}, username)
// and ({username, type = "following"}, target_username)
std::optional<Any> Follow(Any input, StorageAbstraction &);
// Reads a warble thread from the given id, recursively read child threads if
// exist.
std::optional<Any> Read(Any input, StorageAbstraction &);
// Returns this user's following and followers
std::optional<Any> Profile(Any input, StorageAbstraction &);
// Returns the warbles which contain the specified hashtag.
std::optional<Any> WarbleStream(Any input, StorageAbstraction &);
// Check if a user exists
bool hasUser(std::string username, StorageAbstraction &);
} // namespace warble

#endif // WARBLE_FUNCTIONS_H_
