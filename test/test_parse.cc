/*
 * File Name: test_parse.cc
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/27
 *
 */
#include <glog/logging.h>
#include <gtest/gtest.h>

#include <fstream>
#include <cql2cpp/cql2cpp.h>

class ParseTest : public testing::Test {
 protected:
  std::string prefix_;
  std::string case_name_;

 public:
  ParseTest() : prefix_("indoorjson/") {}

  void SetUp() override {
    case_name_ = std::string(
        testing::UnitTest::GetInstance()->current_test_info()->name());
  }

  void TearDown() override {}

  bool Parse(const std::string& case_name) {
    std::ifstream fin(prefix_ + case_name + ".txt");
    EXPECT_TRUE(fin.good());
    EXPECT_TRUE(fin.is_open());
    std::string query;
    query.assign(std::istreambuf_iterator<char>(fin),
                   std::istreambuf_iterator<char>());
    fin.close();

    LOG(INFO) << query;

    std::string dot;
    return cql2cpp::Cql2Cpp<void*>::ToDot(query, &dot, nullptr);
  }
};

// clang-format off
// TEST_F(ParseTest, binary      ) { EXPECT_TRUE(Parse(case_name_)); }  // BBOX
// TEST_F(ParseTest, binlocations) { EXPECT_TRUE(Parse(case_name_)); }  // function
// TEST_F(ParseTest, labels      ) { EXPECT_TRUE(Parse(case_name_)); }  // IN
TEST_F(ParseTest, localization) { EXPECT_TRUE(Parse(case_name_)); }
// clang-format on
