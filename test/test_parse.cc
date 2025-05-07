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

#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

class ParseTest : public testing::Test {
 protected:
  std::string prefix_;
  std::string case_name_;
  bool gen_dot_;

 public:
  ParseTest() : prefix_("indoorjson/"), gen_dot_(false) {}

  void SetUp() override {
    FLAGS_colorlogtostderr = true;
    case_name_ = std::string(
        testing::UnitTest::GetInstance()->current_test_info()->name());
  }

  static std::string hyphen(const std::string& case_name) {
    std::string s = case_name;
    std::replace(s.begin(), s.end(), '_', '-');
    return s;
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
    if (ret and gen_dot_) {
      std::string dot_filename = case_name + ".dot";
      std::ofstream fout(dot_filename);
      if (fout.is_open()) {
        fout << dot;
        fout.close();
        std::stringstream ss;
        ss << "dot -Tpng " << dot_filename << " -o " << case_name << ".png";
        int ret = std::system(ss.str().c_str());
        if (ret != 0) LOG(WARNING) << "run dot command error";
      } else {
        LOG(ERROR) << "Can not open file " << dot_filename;
      }
    }
    return ret;
  }
};

class ParseExampleTest : public ParseTest {
 public:
  ParseExampleTest() : ParseTest() { prefix_ = "supported/1.0/examples/text/"; }
};

class TryAll : public ParseTest {
 public:
  TryAll() : ParseTest() {}

  inline std::vector<std::string> ListFilesInDirectory(
      const std::string& path) {
    std::vector<std::string> result;
    try {
      for (const auto& entry : fs::directory_iterator(path))
        if (fs::is_regular_file(entry.path())) {
          std::string full_path = entry.path().string();
          result.emplace_back(full_path.substr(path.size()));
        }
    } catch (const std::exception& e) {
      std::cerr << "List files in directory Error: " << e.what() << std::endl;
    }
    return result;
  }

  void Run(const std::string& path) {
    prefix_ = path;
    gen_dot_ = true;
    auto files = ListFilesInDirectory(path);
    std::sort(files.begin(), files.end());
    std::vector<std::string> ok;
    std::vector<std::string> error;
    for (const auto& file : files) {
      LOG(WARNING) << file;
      if (Parse(file.substr(0, file.size() - 4))) {
        ok.emplace_back(file);
      } else {
        error.emplace_back(file);
      }
    }

    for (const auto& f : ok) LOG(INFO) << "OK  " << f;
    for (const auto& f : error) LOG(INFO) << "ERR " << f;
  }
};

// clang-format off
TEST_F(ParseTest, bbox          ) { EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseTest, is_in_list    ) { EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseTest, is_not_in_list) { EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseTest, localization  ) { EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseTest, array  )        { EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseTest, binlocations  ) { EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseTest, related_bins  ) { EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseTest, avg           ) { EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseTest, spatial       ) { EXPECT_TRUE(Parse(case_name_)); }
// TEST_F(ParseTest, labels        ) { EXPECT_TRUE(Parse(case_name_)); }  // standard unsupported

TEST_F(ParseExampleTest, clause6_01) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, clause6_02a) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, clause6_02b) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, clause6_02c) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, clause6_03) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, clause7_01) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, clause7_02) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, clause7_03a) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, clause7_03b) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, clause7_07) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, clause7_10) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, clause7_15) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, clause7_16) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, clause7_19) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example01) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example02) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example03) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example04) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example05a) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example05b) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example06a) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example06b) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example07) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example08) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example09) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example10) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example11) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example12) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example13) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example14) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example15) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example16) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example17) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example18) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example19) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example23) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example24) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example25) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example28) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example29) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example30) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example31) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example32) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example33) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example34) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example35) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example36) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example36_alt01) {EXPECT_TRUE(Parse(hyphen(case_name_))); }
TEST_F(ParseExampleTest, example37) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example38) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example38_alt01) {EXPECT_TRUE(Parse(hyphen(case_name_))); }
TEST_F(ParseExampleTest, example39) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example40) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example40_alt01) {EXPECT_TRUE(Parse(hyphen(case_name_))); }
TEST_F(ParseExampleTest, example41) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example42) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example42_alt01) {EXPECT_TRUE(Parse(hyphen(case_name_))); }
TEST_F(ParseExampleTest, example43) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example43_alt01) {EXPECT_TRUE(Parse(hyphen(case_name_))); }
TEST_F(ParseExampleTest, example44) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example44_alt01) {EXPECT_TRUE(Parse(hyphen(case_name_))); }
TEST_F(ParseExampleTest, example46) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example46_alt01) {EXPECT_TRUE(Parse(hyphen(case_name_))); }
TEST_F(ParseExampleTest, example47) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example48) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example49_alt01) {EXPECT_TRUE(Parse(hyphen(case_name_))); }
TEST_F(ParseExampleTest, example50) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example51) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example52) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example68) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example69) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example71) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example72) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example73) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example74) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example75) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example76) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example77) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example78) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example79) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example80) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example81) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example83) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example84) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example85) {EXPECT_TRUE(Parse(case_name_)); }
TEST_F(ParseExampleTest, example85_alt01) {EXPECT_TRUE(Parse(hyphen(case_name_))); }
TEST_F(ParseExampleTest, example86) {EXPECT_TRUE(Parse(case_name_)); }


TEST_F(TryAll, TryAll) { Run("supported/1.0/examples/text/"); }
// clang-format on
