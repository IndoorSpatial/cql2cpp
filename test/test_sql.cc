/*
 * File Name: test_sql.cc
 *
 * Copyright (c) 2024 - 2025 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2025/05/08
 *
 */
#include <gtest/gtest.h>
#include <sqlite3.h>
// #include <spatialite.h>

TEST(WHERE, CASE1) {
  sqlite3 *db;
  sqlite3_open("./test.sqlite", &db);
  char *error_msg = 0;
  sqlite3_exec(db, "PRAGMA strict = OFF;", NULL, 0, &error_msg);
  sqlite3_exec(
      db, "CREATE TABLE IF NOT EXISTS feature(ID INT PRIMARY KEY, NAME TEXT)",
      NULL, 0, &error_msg);

  std::string sql = "SELECT * FROM feature WHERE field = \"Yu\";";
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK)
    std::cerr << "prepare error " << rc << ": " << sqlite3_errmsg(db) << std::endl;
}

