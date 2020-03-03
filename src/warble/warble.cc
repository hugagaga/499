#include <iostream>

#include <gflags/gflags.h>

#include "warble.grpc.pb.h"
#include "../func/func_client.h"
#include "../func/func_infra.h"

using warble::RegisterUserRequest

DEFINE_string(registeruser, "username", "Username to register");

int main(int argc, char **argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  FuncClient warble(grpc::CreateChannel("localhost:50000", grpc::InsecureChannelCredentials()));
  warble.HookAll();
  // TODO: Figure out how to validate flag arguments
  if (!FLAG_registeruser.empty()) {
    RegisterUserRequest request;
    request.set_username(FLAG_registeruser);
    RegisterUserReply reply;
    warble.Event(func::EventType::REGISTER_USER, request, reply);
  }
}


