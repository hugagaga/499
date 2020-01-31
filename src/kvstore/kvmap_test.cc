#include <gtest/gtest.h>

#include "kvmap.h"

#include <vector>



//Test class Kvmap functions
namespace{

//Test get(key)
TEST(KvmapTest, TestGet) {
  std::vector<std::pair<std::string, std::string>> v;
  v.push_back({"banana", "yellow"});
  v.push_back({"apple", "red"});
  Kvmap map(v);
  EXPECT_EQ("red", map.get("apple"));
  EXPECT_EQ("yellow", map.get("banana"));
  EXPECT_EQ("NOT_FOUND", map.get("dog"));
}

//Test put(key, value)
TEST(PutTest, TestPut) {
  Kvmap map;
  EXPECT_EQ("NOT_FOUND", map.get("banana"));
  map.put("banana","yellow");
  EXPECT_EQ("yellow", map.get("banana"));
}

//Test remove(key)
TEST(RemoveTest, TestRemove) {
  std::vector<std::pair<std::string, std::string>> v;
  v.push_back({"banana", "yellow"});
  Kvmap map(v);
  EXPECT_EQ("yellow", map.get("banana"));
  map.remove("banana");
  EXPECT_EQ("NOT_FOUND", map.get("banana"));
}

}  //namespace

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
