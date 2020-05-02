#include "warble_func.h"

#include <unistd.h>

#include <gtest/gtest.h>

#include "../kvstore/kvstore_local.h"
#include "warble.grpc.pb.h"

using warble::RegisteruserReply;
using warble::RegisteruserRequest;
using warble::StreamReply;
using warble::StreamRequest;
using warble::Timestamp;
using warble::WarbleReply;
using warble::WarbleRequest;

// Test: WarbleStream
// Expected: Return expected warbles that contain requested hashtag and are
// posted after requested time
TEST(WarbleFunc, shouldReturnExpectedWarblesWhenRequestWarbleStream) {
  // mock local KeyValueStorage
  KeyValueStoreLocal mock_store;

  // register mocked user
  RegisteruserRequest register_request;
  register_request.set_username("user1");
  Any register_input;
  register_input.PackFrom(register_request);
  warble::RegisterUser(register_input, mock_store);
  ASSERT_TRUE(warble::hasUser("user1", mock_store));

  // mock new warble 1 with hashtag #red
  WarbleRequest request;
  Any input;
  request.set_username("user1");
  request.set_text("#red It's my first warble.");
  input.PackFrom(request);
  std::optional<Any> output1 = warble::WarbleFunc(input, mock_store);
  usleep(1000);
  // mock new warble 2 with hashtag #black
  request.set_username("user1");
  request.set_text("#black It's my second warble.");
  input.PackFrom(request);
  std::optional<Any> output2 = warble::WarbleFunc(input, mock_store);
  usleep(1000);
  // mock new warble 3 with hashtag #red
  request.set_username("user1");
  request.set_text("#red It's my third warble.");
  input.PackFrom(request);
  std::optional<Any> output3 = warble::WarbleFunc(input, mock_store);
  usleep(1000);
  // mock new warble 4 with hashtag #black
  request.set_username("user1");
  request.set_text("#black It's my forth warble.");
  input.PackFrom(request);
  std::optional<Any> output4 = warble::WarbleFunc(input, mock_store);
  usleep(1000);
  // mock new warble 5 with hashtag #red
  request.set_username("user1");
  request.set_text("#red It's my fifth warble.");
  input.PackFrom(request);
  std::optional<Any> output5 = warble::WarbleFunc(input, mock_store);
  usleep(1000);
  // use warble1's time as mock requested time
  ASSERT_TRUE(output1.has_value());
  WarbleReply warble1;
  output1.value().UnpackTo(&warble1);
  Timestamp mock_request_time = warble1.warble().timestamp();

  // mock WarbleStream Request
  // request warbleStream for hashtag #red and hashtag #black separately.
  StreamRequest stream_request;
  stream_request.set_hashtag("red");
  stream_request.mutable_timestamp()->CopyFrom(mock_request_time);
  input.PackFrom(stream_request);
  std::optional<Any> output_red = warble::WarbleStream(input, mock_store);

  ASSERT_TRUE(output_red.has_value());
  StreamReply stream_red;
  output_red.value().UnpackTo(&stream_red);

  ASSERT_EQ(stream_red.warbles_size(), 2);
  EXPECT_EQ(stream_red.warbles(0).text(), "#red It's my third warble.");
  EXPECT_EQ(stream_red.warbles(1).text(), "#red It's my fifth warble.");

  stream_request.set_hashtag("black");
  input.PackFrom(stream_request);
  std::optional<Any> output_black = warble::WarbleStream(input, mock_store);

  ASSERT_TRUE(output_black.has_value());
  StreamReply stream_black;
  output_black.value().UnpackTo(&stream_black);

  ASSERT_EQ(stream_black.warbles_size(), 2);
  EXPECT_EQ(stream_black.warbles(0).text(), "#black It's my second warble.");
  EXPECT_EQ(stream_black.warbles(1).text(), "#black It's my forth warble.");
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}