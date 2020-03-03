#include "warble_func.h"

#include <string>

#include "warble.grpc.pb.h"
#include "kvstore_client.h"

using kvstore::KeyValueStore;
using kvstore::PutReply;
using kvstore::PutRequest;

namespace warble {
  bool Functions::RegisterUser(std::string username) {
    RegisteruserRequest request;
    request.set_username(username);
    KeyValueStoreClient func(grpc::CreateChannel(
      "localhost:50001", grpc::InsecureChannelCredentials()));
    func.Put(currentUserId_,username);
  }
} // namespace warble