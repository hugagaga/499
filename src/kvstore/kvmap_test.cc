#include "kvmap.h"

#include <vector>

#include <gtest/gtest.h>

// Test class Kvmap functions
namespace {

// Test fixture class for setting up initial values for multiple test cases
class KvmapTest : public ::testing::Test {
 protected:
  // Set up a kvmap for every test {(banana, yellow), (apple, red), (apple,
  // round)}
  void SetUp() override {
    std::vector<std::pair<std::string, std::string>> v;
    v.push_back({"banana", "yellow"});
    v.push_back({"apple", "red"});
    v.push_back({"apple", "round"});
    map.KvmapSetup(v);
  }

  kvstore::Kvmap map;
};

// Test if Get(key)
// Gets the correct value when the value of that key is previously stored in the
// key value map; return NOT_FOUND = -1 signal if the key does not exist.
TEST_F(KvmapTest, GetMultipleExistingValue) {
  ASSERT_TRUE(map.Get("apple"));
  ASSERT_EQ(2, map.Get("apple").value().size());
  auto values = map.Get("apple").value();
  bool result = (values[0] == "round" || values[1] == "round") &&
                (values[0] == "red" || values[1] == "red");
  ASSERT_EQ("yellow", map.Get("banana").value()[0]);
}

// Test if Get non-existing value returns false
TEST_F(KvmapTest, GetNonExistingValue) { ASSERT_FALSE(map.Get("dog")); }

// Test Put(key, value) puts <key, value> pair into the map
TEST(PutTest, TestPut) {
  kvstore::Kvmap map;
  EXPECT_FALSE(map.Get("banana").has_value());
  map.Put("banana", "yellow");
  EXPECT_EQ("yellow", map.Get("banana").value()[0]);
}

// Test Remove(key) Removes the value in the map, given the
// Get(key) function works properly
TEST_F(KvmapTest, TestRemove) {
  EXPECT_TRUE(map.Get("apple"));
  EXPECT_EQ(2, map.Remove("apple"));
  EXPECT_EQ(std::nullopt, map.Get("apple"));
}

//Since the data is serialized by protobuf, we can not test
//Update(filename) and Restore(filename) seperately by comparing file contents.

//Test if Update serializes kvmap to a file and Restore can restore the previous kvmap from
//the serialized string in a existing File
TEST(UpdateRestoreTest, RestoreExistingFile) {
  kvstore::Kvmap map1;
  map1.Put("apple", "red");
  map1.Update("testfile");
  kvstore::Kvmap map2;
  map2.Restore("testfile");
  EXPECT_EQ("red", map2.Get("apple").value()[0]);
}

//Test Restore corner case where the Restore function has a non-existing file to restore.
//The restored kvmap should be empty
TEST(UpdateRestoreTest, RestoreNonExistingFile) {
  kvstore::Kvmap map;
  map.Restore("testfile2");
  EXPECT_EQ(std::nullopt, map.Get("apple"));
}

}  // namespace

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
