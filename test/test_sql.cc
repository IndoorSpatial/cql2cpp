/*
 * File Name: test_sql.cc
 *
 * Copyright (c) 2024 - 2025 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2025/05/08
 *
 */
#include <cql2cpp/cql2cpp.h>
#include <gtest/gtest.h>
#include <sqlite3.h>
//
#include <spatialite.h>

#include <fstream>

TEST(SQL_SPATIAL, CASE1) {
  spatialite_init(0);
  sqlite3 *db;
  sqlite3_open("./test.sqlite", &db);
  char *error_msg = 0;

  int ret;

  ret = sqlite3_enable_load_extension(db, 1);
  EXPECT_EQ(ret, SQLITE_OK);

  ret = sqlite3_load_extension(db, "mod_spatialite.so", NULL, NULL);

  ret = sqlite3_exec(db, "PRAGMA strict = OFF;", NULL, 0, &error_msg);
  if (ret != SQLITE_OK) LOG(ERROR) << error_msg;
  EXPECT_EQ(ret, SQLITE_OK);

  ret = sqlite3_exec(db, "SELECT spatialite_version();", NULL, 0, &error_msg);
  if (ret != SQLITE_OK) LOG(ERROR) << error_msg;
  EXPECT_EQ(ret, SQLITE_OK);

  ret = sqlite3_exec(db,
                     "CREATE TABLE IF NOT EXISTS feature(ID INT PRIMARY KEY);",
                     NULL, 0, &error_msg);
  if (ret != SQLITE_OK) LOG(ERROR) << error_msg;
  EXPECT_EQ(ret, SQLITE_OK);

  ret = sqlite3_exec(
      db, "SELECT AddGeometryColumn('feature', 'geom', 4326, 'POLYGON', 'XY');",
      NULL, 0, &error_msg);
  if (ret != SQLITE_OK) LOG(ERROR) << error_msg;
  EXPECT_EQ(ret, SQLITE_OK);

  ret = sqlite3_exec(db,
                     "SELECT * FROM feature WHERE ST_INTERSECTS('geom' "
                     ",ST_GeomFromText('POINT(36 32)'));",
                     NULL, 0, &error_msg);
  if (ret != SQLITE_OK) LOG(ERROR) << error_msg;
  EXPECT_EQ(ret, SQLITE_OK);
}

class SqlTest : public testing::Test {
 protected:
  std::string case_name_;
  std::string prefix_;

 public:
  SqlTest() { prefix_ = "supported/1.0/examples/text/"; }
  void SetUp() override {
    FLAGS_colorlogtostderr = true;
    case_name_ = std::string(
        testing::UnitTest::GetInstance()->current_test_info()->name());
    spatialite_init(0);
  }
  void TearDown() override {}
  static std::string hyphen(const std::string &case_name) {
    std::string s = case_name;
    std::replace(s.begin(), s.end(), '_', '-');
    return s;
  }

  bool Convert(const std::string &case_name) {
    std::ifstream fin(prefix_ + case_name + ".txt");
    EXPECT_TRUE(fin.good());
    EXPECT_TRUE(fin.is_open());
    std::string query;
    query.assign(std::istreambuf_iterator<char>(fin),
                 std::istreambuf_iterator<char>());
    fin.close();

    LOG(WARNING) << "CQL2: " << query;

    // Create DB
    sqlite3 *db;
    sqlite3_open("./test.sqlite", &db);
    char *error_msg = 0;
    sqlite3_enable_load_extension(db, 1);
    sqlite3_load_extension(db, "mod_spatialite.so", NULL, NULL);
    sqlite3_exec(db, "PRAGMA strict = OFF;", NULL, 0, &error_msg);
    sqlite3_exec(db, "SELECT load_extension('mod_spatialite');", NULL, 0,
                 &error_msg);
    sqlite3_exec(db,
                 "CREATE TABLE IF NOT EXISTS feature(ID INT PRIMARY KEY, "
                 "PLACEHOLDER TEXT)",
                 NULL, 0, &error_msg);

    // Parse query to get queryables
    cql2cpp::Cql2Cpp<void *> cql2cpp;
    std::string error_str;
    auto root = cql2cpp.ParseAsAst(query, &error_str);

    // add queryables into table
    std::set<std::string> queryables;
    for (const auto &node : *root) {
      if (node->type() == cql2cpp::NodeType::PropertyName)
        queryables.insert(value_str(node->origin_value()));
    }
    for (const auto &queryable : queryables)
      sqlite3_exec(
          db,
          ("ALTER TABLE feature ADD COLUMN " + queryable + " TEXT;").c_str(),
          NULL, 0, &error_msg);

    std::string sql_where;
    std::string err_str;
    EXPECT_TRUE(cql2cpp.ConvertToSQL(query, &sql_where, &err_str));

    LOG(WARNING) << "SQL:  " << sql_where;

    // use "prepare" to check the WHERE clause
    std::string sql = "SELECT * FROM feature WHERE " + sql_where;
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
      LOG(ERROR) << "prepare error " << rc << ": " << sqlite3_errmsg(db)
                 << std::endl;
    }
    return rc == SQLITE_OK;
  }
};

// clang-format off

// TEST_F(SqlTest, clause6_01) { EXPECT_TRUE(Convert(case_name_)); }  // function
// TEST_F(SqlTest, clause6_02b) { EXPECT_TRUE(Convert(case_name_)); }  // function
// TEST_F(SqlTest, example68) { EXPECT_TRUE(Convert(case_name_)); }  // function
// TEST_F(SqlTest, example69) { EXPECT_TRUE(Convert(case_name_)); }  // function

// TEST_F(SqlTest, example28) { EXPECT_TRUE(Convert(case_name_)); }  // CASEI
// TEST_F(SqlTest, example71) { EXPECT_TRUE(Convert(case_name_)); }  // CASEI
// TEST_F(SqlTest, example86) { EXPECT_TRUE(Convert(case_name_)); }  // CASEI

// TEST_F(SqlTest, clause7_15) { EXPECT_TRUE(Convert(case_name_)); }  // array
// TEST_F(SqlTest, example79) { EXPECT_TRUE(Convert(case_name_)); }  // array
// TEST_F(SqlTest, example80) { EXPECT_TRUE(Convert(case_name_)); }  // array
// TEST_F(SqlTest, example81) { EXPECT_TRUE(Convert(case_name_)); }  // array

TEST_F(SqlTest, clause6_02a) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, clause6_02c) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, clause6_03) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, clause7_01) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, clause7_02) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, clause7_03a) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, clause7_03b) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, clause7_07) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, clause7_10) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, clause7_16) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, clause7_19) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example01) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example02) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example03) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example04) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example05a) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example05b) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example06a) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example06b) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example07) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example08) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example09) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example10) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example11) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example12) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example13) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example14) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example15) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example16) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example17) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example18) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example19) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example23) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example24) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example25) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example29) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example30) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example31) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example32) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example33) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example34) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example35) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example36) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example36_alt01) { EXPECT_TRUE(Convert(hyphen(case_name_))); }
TEST_F(SqlTest, example37) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example38) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example38_alt01) { EXPECT_TRUE(Convert(hyphen(case_name_))); }
TEST_F(SqlTest, example39) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example40) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example40_alt01) { EXPECT_TRUE(Convert(hyphen(case_name_))); }
TEST_F(SqlTest, example41) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example42) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example42_alt01) { EXPECT_TRUE(Convert(hyphen(case_name_))); }
TEST_F(SqlTest, example43) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example43_alt01) { EXPECT_TRUE(Convert(hyphen(case_name_))); }
TEST_F(SqlTest, example44) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example44_alt01) { EXPECT_TRUE(Convert(hyphen(case_name_))); }
TEST_F(SqlTest, example46) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example46_alt01) { EXPECT_TRUE(Convert(hyphen(case_name_))); }
TEST_F(SqlTest, example47) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example48) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example49_alt01) { EXPECT_TRUE(Convert(hyphen(case_name_))); }
TEST_F(SqlTest, example50) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example51) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example52) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example72) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example73) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example74) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example75) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example76) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example77) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example78) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example83) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example84) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example85) { EXPECT_TRUE(Convert(case_name_)); }
TEST_F(SqlTest, example85_alt01) { EXPECT_TRUE(Convert(hyphen(case_name_))); }
