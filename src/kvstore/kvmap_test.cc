#include <gtest/gtest.h>

#include "kvmap.h"

#include <vector>



//Test class Kvmap functions
namespace {

//Test if Get(key) 
//Gets the correct value when the value of that key is previously stored in the key value map; 
//return NOT_FOUND = -1 signal if the key does not exist.
TEST(KvmapTest, TestGet) {
  std::vector<std::pair<std::string, std::string>> v;
  v.push_back({"banana", "yellow"});
  v.push_back({"apple", "red"});
  Kvmap map(v);
  EXPECT_EQ("red", map.Get("apple").first);
  EXPECT_EQ(0, map.Get("apple").second);
  EXPECT_EQ("yellow", map.Get("banana").first);
  EXPECT_EQ(-1, map.Get("dog").second);
}

//Test Put(key, value) puts <key, value> pair into the map
TEST(PutTest, TestPut) {
  Kvmap map;
  EXPECT_EQ(-1, map.Get("banana").second);
  map.Put("banana","yellow");
  EXPECT_EQ("yellow", map.Get("banana").first);
}

//Test Remove(key) Removes the value in the map, given the 
//Get(key) function works properly
TEST(RemoveTest, TestRemove) {
  std::vector<std::pair<std::string, std::string>> v;
  v.push_back({"banana", "yellow"});
  Kvmap map(v);
  EXPECT_EQ("yellow", map.Get("banana").first);
  map.Remove("banana");
  EXPECT_EQ(-1, map.Get("banana").second);
}

}  //namespace

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
