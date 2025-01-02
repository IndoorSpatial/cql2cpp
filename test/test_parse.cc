/*
 * File Name: test_parse.cc
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/27
 *
 */
#include <cql2cpp/cql2cpp.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

#include <fstream>

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
    cql2cpp::AstNode::set_ostream(&std::cout);
    bool ret = cql2cpp::Cql2Cpp<void*>::ToDot(query, &dot, nullptr);
    if (ret) LOG(INFO) << dot;

    std::string dot_filename = case_name + ".dot";
    // std::ofstream fout(dot_filename);
    // if (fout.is_open()) {
    //   fout << dot;
    //   fout.close();
    // } else {
    //   LOG(ERROR) << "Can not open file " << dot_filename;
    // }
    return ret;
  }
};

// clang-format off
TEST_F(ParseTest, binary        ) { EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseTest, is_in_list    ) { EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseTest, is_not_in_list) { EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseTest, localization  ) { EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseTest, array  )        { EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseTest, binlocations  ) { EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseTest, related_bins  ) { EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseTest, avg           ) { EXPECT_TRUE(Parse(case_name_)); }
// TEST_F(ParseTest, labels        ) { EXPECT_TRUE(Parse(case_name_)); }  // standard unsupported
// clang-format on
