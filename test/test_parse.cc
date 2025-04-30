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

#include <filesystem>
#include <fstream>
#include <cstdlib>

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
        if (ret != 0)
          LOG(WARNING) << "run dot command error";
      } else {
        LOG(ERROR) << "Can not open file " << dot_filename;
      }
    }
    return ret;
  }
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
      if (Parse(file.substr(0, file.size() - 4)))
        ok.emplace_back(file);
      else
        error.emplace_back(file);
    }

    for (const auto& f : ok)
      LOG(INFO) << "OK  " << f;
    for (const auto& f : error)
      LOG(INFO) << "ERR " << f;
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

TEST_F(TryAll, TryAll) {
  Run("supported/1.0/examples/text/");
  // Run("unsupported/1.0/examples/text/");
}
// clang-format on
