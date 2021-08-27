#include <catch2/catch_test_macros.hpp>
#include "../unordered_map.h"
#include <set>

using std::pair;
using std::vector;
using std::set;

TEST_CASE( "Test contructors", "[hash-map]" ) {
  vector<pair<int, int>> test_vector = {{1, 1}, {2, 2}, {3,3 }};
  HashMap<int, int> map_simple;
  HashMap<int, int> map_from_vector(test_vector.begin(), test_vector.end());
  HashMap<int, int> map_from_init_list = {{1,1}, {2,2}, {3,3}};

  REQUIRE(map_simple.Size() == 0);
  REQUIRE(map_from_vector.Size() == 3);
  REQUIRE(map_from_init_list.Size() == 3);
}

TEST_CASE("Test inserts, erase, empty", "[hash-map]") {
  HashMap<int, int> map;
  map.Insert({1, 1});
  map.Insert({2, 2});
  REQUIRE(map.Size() == 2);
  map.Erase(1);
  map.Erase(2);
  map.Erase(2); // nothing bad
  REQUIRE(map.Size() == 0);
  REQUIRE(map.Empty());
  REQUIRE_NOTHROW(map.Erase(1));
}

TEST_CASE("Test overwrite insert, clear", "[hash-map]") {
  HashMap<int, int>  map;
  map.Insert({1, 1});
  map.Insert({1, 2});
  REQUIRE(map.Find(1)->second == 2);
  REQUIRE(map.Find(1)->first == 1);
  map.Clear();
  REQUIRE(map.Empty());
}

TEST_CASE("Test [], At") {
  HashMap<int, vector<int>> map;

  map.Insert({1, {1}});
  REQUIRE(map[1] == std::vector<int>{1});
  REQUIRE(map[2] == std::vector<int>());

  REQUIRE_THROWS(map.At(3));
  REQUIRE(map.At(2) == std::vector<int>());
}

TEST_CASE("Returns same vector") {
  pair<int, vector<int>> empty_pair{1, vector<int>()};
  HashMap<int, vector<int>> map;
  map.Insert(empty_pair);
  REQUIRE(&map[1] == &map[1]);
}

TEST_CASE("Iterators") {
  HashMap<int, vector<int>> map;
  REQUIRE(map.Begin() == map.End());
  map.Insert({1, {1}});
  map.Insert({2, {2}});
  set<vector<int>> res;
  for (auto it = map.Begin(); it != map.End(); it++) {
    res.insert(it->second);
  }
  REQUIRE(res == set<vector<int>>{{1}, {2}});
  for (auto it = map.Begin(); it != map.End(); it++) {
    it->second = {1};
  }
  res.clear();
  for (auto it = map.Begin(); it != map.End(); it++) {
    res.insert(it->second);
  }
  REQUIRE(res == set<vector<int>>{{1}, {1}});
}

TEST_CASE("Const iterators") {
  const HashMap<int, vector<int>> map = {{1, {1}}, {10, {10}}};
  set<vector<int>> res;
  for (auto it = map.Begin(); it != map.End(); it++) {
    res.insert(it->second);
  }
  REQUIRE(res == set<vector<int>>{{1}, {10}});
}

TEST_CASE("Begin works correctly") {
  HashMap<int, int> map;
  map.Insert({999, 1});
  REQUIRE(map.Begin()->second == 1);
  REQUIRE(++map.Begin() == map.End());
}








