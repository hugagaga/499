#include <vector>

#include <gtest/gtest.h>

#include "kvmap.h"





//Test class Kvmap functions
namespace {

//Test if Get(key) 
//Gets the correct value when the value of that key is previously stored in the key value map; 
//return NOT_FOUND = -1 signal if the key does not exist.
TEST(KvmapTest, TestGet) {
  std::vector<std::pair<std::string, std::string>> v;
  v.push_back({"banana", "yellow"});
  v.push_back({"apple", "red"});
  v.push_back({"apple", "round"});
  Kvmap map(v);
  EXPECT_TRUE(map.Get("apple"));
  EXPECT_EQ(2, map.Get("apple").value().size());
  EXPECT_EQ("yellow", map.Get("banana").value()[0]);
  EXPECT_FALSE(map.Get("dog"));
}

//Test Put(key, value) puts <key, value> pair into the map
TEST(PutTest, TestPut) {
  Kvmap map;
  EXPECT_FALSE(map.Get("banana").has_value());
  map.Put("banana","yellow");
  EXPECT_EQ("yellow", map.Get("banana").value()[0]);
}

//Test Remove(key) Removes the value in the map, given the 
//Get(key) function works properly
TEST(RemoveTest, TestRemove) {
  std::vector<std::pair<std::string, std::string>> v;
  v.push_back({"banana", "yellow"});
  v.push_back({"apple", "red"});
  std::vector<std::pair<std::string, std::string>> v2(v);
  v.push_back({"apple", "round"});
  Kvmap map(v);
  EXPECT_TRUE(map.Get("apple"));
  EXPECT_EQ(2, map.Remove("apple"));
  EXPECT_EQ(std::nullopt, map.Get("apple"));
}

}  //namespace

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
