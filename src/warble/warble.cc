#include <map>
#include <stack>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include "../func/func_client.h"
#include "../func/func_infra.h"
#include "../func/warble.grpc.pb.h"

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
using warble::Timestamp;
using warble::WarbleReply;
using warble::WarbleRequest;

DEFINE_string(registeruser, "", "Username to register");
DEFINE_string(warble, "", "Text of a warble");
DEFINE_string(user, "", "Log in as User");
DEFINE_string(reply, "", "Reply warble id");
DEFINE_string(follow, "", "Follow the given username");
DEFINE_string(read, "", "Warble thread id");
DEFINE_bool(profile, false, "Boolean input or empty as ture");

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);
  FuncClient warble(grpc::CreateChannel("localhost:50000",
                                        grpc::InsecureChannelCredentials()));
  if (FLAGS_user.empty()) {
    // Register User Event
    if (!FLAGS_registeruser.empty()) {
      warble.Hook(func::EventType::REGISTER_USER);
      RegisteruserRequest request;
      request.set_username(FLAGS_registeruser);
      Any* input = new Any();
      input->PackFrom(request);
      Any* output = new Any();
      Status status =
          warble.Event(func::EventType::REGISTER_USER, input, output);
      RegisteruserReply reply;
      output->UnpackTo(&reply);
      if (status.ok()) {
        std::cout << "User: " << FLAGS_registeruser
                  << " successfully registerred!" << std::endl;
      } else if (status.error_code() == grpc::StatusCode::INVALID_ARGUMENT) {
        std::cout << "Username already exists!" << std::endl;
      } else {
        std::cout << "Func server error!" << std::endl;
      }
      warble.Unhook(func::EventType::REGISTER_USER);
    }
  } else {
    // Warble(reply) event
    if (!FLAGS_warble.empty()) {
      warble.Hook(func::EventType::WARBLE);
      WarbleRequest request;
      Any* input = new Any();
      Any* output = new Any();
      request.set_username(FLAGS_user);
      request.set_text(FLAGS_warble);
      bool isReply = !FLAGS_reply.empty();
      if (isReply) {
        request.set_parent_id(FLAGS_reply);
      }
      input->PackFrom(request);

      Status status = warble.Event(func::EventType::WARBLE, input, output);

      WarbleReply reply;
      output->UnpackTo(&reply);
      warble::Warble returned_warble = reply.warble();
      Timestamp timestamp = returned_warble.timestamp();
      int time = timestamp.seconds();
      std::time_t t = static_cast<std::time_t>(time);

      // Print out detailed warble info to the user
      if (status.ok()) {
        std::cout << "Warble: " << FLAGS_warble << " successfully posted!"
                  << std::endl;
        std::cout << "---------------------------------------------------"
                  << std::endl;
        std::cout << "Username: " << returned_warble.username() << std::endl;
        std::cout << "warble ID: " << returned_warble.id() << std::endl;
        if (isReply) {
          std::cout << "parent warble ID: " << returned_warble.parent_id()
                    << std::endl;
        }
        std::cout << "Time: " << std::asctime(std::localtime(&t)) << std::endl;
        std::cout << "Content: " << returned_warble.text() << std::endl;
        std::cout << "---------------------------------------------------"
                  << std::endl;
      } else {
        std::cout << "service not available!" << std::endl;
      }
      warble.Unhook(func::EventType::WARBLE);
    }
    // Follow Event
    if (!FLAGS_follow.empty()) {
      if (FLAGS_user == FLAGS_follow) {
        std::cout << "You can not follow yourself!";
      } else {
        warble.Hook(func::EventType::FOLLOW);
        FollowRequest request;
        request.set_username(FLAGS_user);
        request.set_to_follow(FLAGS_follow);
        Any* input = new Any();
        input->PackFrom(request);
        Any* output = new Any();
        Status status = warble.Event(func::EventType::FOLLOW, input, output);
        FollowReply reply;
        output->UnpackTo(&reply);

        if (status.ok()) {
          std::cout << "User " << FLAGS_user << " successfully followed "
                    << FLAGS_follow << std::endl;
        } else {
          std::cout << "User does not exist" << std::endl;
        }
      }
      warble.Unhook(func::EventType::FOLLOW);
    }
    // Read Event
    if (!FLAGS_read.empty()) {
      warble.Hook(func::EventType::READ);
      ReadRequest request;
      request.set_warble_id(FLAGS_read);
      request.set_user(FLAGS_user);
      Any* input = new Any();
      input->PackFrom(request);
      Any* output = new Any();
      Status status = warble.Event(func::EventType::READ, input, output);
      ReadReply reply;
      output->UnpackTo(&reply);

      if (status.ok()) {
        // Reorganize reply with DFS traversal and sort based on time
        warble::Warble source;
        std::map<std::string, warble::Warble> warbles;
        // (parent_id, warble) multimap
        std::multimap<std::string, warble::Warble> warbles_p;
        std::map<std::string, bool> visited;
        std::stack<std::pair<warble::Warble, int>> s;
        for (int i = 0; i < reply.warbles_size(); ++i) {
          if (reply.warbles(i).id() == FLAGS_read) {
            source = reply.warbles(i);
          }
          warbles.insert({reply.warbles(i).id(), reply.warbles(i)});
          visited.insert({reply.warbles(i).id(), false});
        }
        // Adjacent list
        for (int i = 0; i < reply.warbles_size(); ++i) {
          warbles_p.insert({reply.warbles(i).parent_id(), reply.warbles(i)});
        }
        // DFS
        int indent = 0;
        visited[source.id()] = true;
        s.push({source, indent});
        std::cout << "Warble thread (id)" << FLAGS_read
                  << " below:" << std::endl
                  << std::endl;
        while (!s.empty()) {
          warble::Warble w = s.top().first;
          indent = s.top().second;
          s.pop();

          std::string space;
          for (int i = 0; i < indent; i++) {
            space += " ";
          }
          space += "|";
          Timestamp timestamp = w.timestamp();
          int time = timestamp.seconds();
          std::time_t t = static_cast<std::time_t>(time);
          std::cout << space << "---------------------------------------------------"
                    << std::endl;
          std::cout << space << "Username: " << w.username() << std::endl;
          std::cout << space << "warble ID: " << w.id() << std::endl;
          if (w.id() != source.id()) {
            std::cout << space << "parent warble ID: " << w.parent_id()
                      << std::endl;
          }
          std::cout << space << "Time: " << std::asctime(std::localtime(&t));
          std::cout << space << "Content: " << w.text() << std::endl;
          std::cout << space << "---------------------------------------------------"
                    << std::endl;
          auto range = warbles_p.equal_range(w.id());
          if (range.first != warbles_p.end()) {
            std::map<int, warble::Warble, std::greater<int>> subtree;
            for (auto it = range.first; it != range.second; ++it) {
              subtree.insert({it->second.timestamp().seconds(), it->second});
            }
            for (auto it = subtree.begin(); it != subtree.end(); it++) {
              if (!visited[it->second.id()]) {
                visited[it->second.id()] = true;
                s.push({it->second, indent + 4});
              }
            }
          }
        }
      } else {
        std::cout << "Warble thread does not exist" << std::endl;
      }
      warble.Unhook(func::EventType::READ);
    }

    if (FLAGS_profile) {
      warble.Hook(func::EventType::PROFILE);
      ProfileRequest request;
      request.set_username(FLAGS_user);
      Any* input = new Any();
      input->PackFrom(request);
      Any* output = new Any();
      Status status = warble.Event(func::EventType::PROFILE, input, output);
      ProfileReply reply;
      output->UnpackTo(&reply);
      if (status.ok()) {
        std::cout << "Profile of " << FLAGS_user << " :" << std::endl;
        std::cout << "Follers: " << std::endl;
        for (int i = 0; i < reply.followers_size(); ++i) {
          std::cout << reply.followers(i) << std::endl;
        }
        std::cout << "Following: " << std::endl;
        for (int i = 0; i < reply.following_size(); ++i) {
          std::cout << reply.following(i) << std::endl;
        }
      } else {
        std::cout << "User does not exist" << std::endl;
      }
      warble.Unhook(func::EventType::PROFILE);
    }
  }
}
