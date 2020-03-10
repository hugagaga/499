#include <iostream>
#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "../func/warble.grpc.pb.h"
#include "../func/func_client.h"
#include "../func/func_infra.h"

using google::protobuf::Any;
using grpc::Status;
using warble::FollowReply;
using warble::FollowRequest;
using warble::ProfileReply;
using warble::ProfileRequest;
using warble::ReadReply;
using warble::ReadRequest;
using warble::RegisteruserReply;
using warble::RegisteruserRequest;
using warble::WarbleReply;
using warble::WarbleRequest;

DEFINE_string(registeruser, "username", "Username to register");
DEFINE_string(warble, "warble", "Text of a warble");
DEFINE_string(user, "username", "Log in as User");
DEFINE_string(reply, "reply warble id", "Reply warble id");
DEFINE_string(follow, "follow the given username", "Follow the given username");
DEFINE_string(read, "read warble thread", "Warble thread id");
DEFINE_bool(profile, "read following and followers", "Boolean input or empty as ture");

int main(int argc, char** argv) {
  FLAGS_log_dir = "/499/log";
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);
  FuncClient warble(grpc::CreateChannel("localhost:50000", grpc::InsecureChannelCredentials()));
  //warble.HookAll();

  if (!FLAGS_registeruser.empty()) {
    warble.Hook(func::EventType::REGISTER_USER);
    RegisteruserRequest request;
    request.set_username(FLAGS_registeruser);
    Any* input = new Any();
    input->PackFrom(request);
    Any* output = new Any();
    Status status = warble.Event(func::EventType::REGISTER_USER, input, output);
    RegisteruserReply reply;
    output->UnpackTo(&reply);
    if (status.ok()) {
      std::cout << "User: " << FLAGS_registeruser << " successfully registerred!" << std::endl;
    } else {
      std::cout << "Username already exists!";
    }
  }

  if (!FLAGS_warble.empty()) {
    if (!FLAGS_user.empty()) {
      warble.Hook(func::EventType::WARBLE);
      WarbleRequest request;
      request.set_username(FLAGS_user);
      request.set_text(FLAGS_warble);
      if (!FLAGS_reply.empty()) {
        request.set_parent_id = FLAGS_reply;
      }
      Any* input = new Any();
      input->PackFrom(request);
      Any* output = new Any();
      Status status = warble.Event(func::EventType::WARBLE, input, output);
      WarbleReply reply;
      output->UnpackTo(&reply);
      if (status.ok()) {
        std::cout << "Warble: " << FLAGS_warble << " successfully posted!" << std::endl;
      } else {
        std::cout << "service not available!" << std::endl;
      }
    }
  }

  if (!FLAGS_follow.empty()) {
    if (!FLAGS_user.empty()) {
      warble.Hook(func::EventType::FOLLOW);
      FollowRequest request;
      request.set_username(FLAGS_user);
      request.set_to_follow(FLAGS_follow);
      Any* input = new Any();
      input->PackFrom(request);
      Any* output = new Any();
      Status status = warble.Event(func::EventType::WARBLE, input, output);
      FollowReply reply;
      reply->UnpackTo(&reply);
      if (status.ok()) {
        std::cout << "Successfully followed " << FLAGS_follow << std::endl;
      } else {
        std::cout << "User does not exist" << std::endl;
      }
    }
  }

  if (!FLAGS_read.empty()) {
    if (!FLAG_user.empty()) {
      warble.Hook(func::EventType::READ);
      ReadRequest request;
      request.set_warble_id(FLAG_read);
      Any* input = new Any();
      input->PackFrom(request);
      Any* output = new Any();
      Status status = warble.Event(func::EventType::READ, input, output);
      ReadReply reply;
      reply->UnpackTo(&reply);
      if (status.ok()) {
        std::cout << "Warble thread (id)" << FLAGS_follow << " below:" std::endl;
        //TODO:: repeated;
      } else {
        std::cout << "Warble does not exist" << std::endl;
      }
    }
  }

  if(!FLAGS_profile.empty()) {
    if (!FLAGS_user.empty()) {
      warble.Hook(func::EventType::PROFILE);
      ProfileRequest request;
      request.set_username(FLAGS_user);
      Any* input = new Any();
      input->PackFrom(request);
      Any* output = new Any();
      Status status = warble.Event(func::EventType::READ, input, output);
      ProfileReply reply;
    }    
  }
}
