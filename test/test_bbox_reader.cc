/*
 * File Name: test_bbox_reader.cc
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/27
 *
 */
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <cql2cpp/bbox_reader.h>

TEST(BBOX_READER, CASE1) {
  std::string input = "BBOX(00,11,22,33)";
  auto env = cql2cpp::BBoxReader().read(input);
  EXPECT_NEAR(env->getMinX(), 0, 1e-6);
  EXPECT_NEAR(env->getMinY(), 11, 1e-6);
  EXPECT_NEAR(env->getMaxX(), 22, 1e-6);
  EXPECT_NEAR(env->getMaxY(), 33, 1e-6);
}

TEST(BBOX_READER, CASE2) {
  std::string input = "BBOX ( 00, 11, 22 ,33 )";
  auto env = cql2cpp::BBoxReader().read(input);
  EXPECT_NEAR(env->getMinX(), 0, 1e-6);
  EXPECT_NEAR(env->getMinY(), 11, 1e-6);
  EXPECT_NEAR(env->getMaxX(), 22, 1e-6);
  EXPECT_NEAR(env->getMaxY(), 33, 1e-6);
}

TEST(BBOX_READER, CASE3) {
  std::string input = "BBOX ( -01.0, +11, 22, +33.0 )";
  auto env = cql2cpp::BBoxReader().read(input);
  EXPECT_NEAR(env->getMinX(), -1, 1e-6);
  EXPECT_NEAR(env->getMinY(), 11, 1e-6);
  EXPECT_NEAR(env->getMaxX(), 22, 1e-6);
  EXPECT_NEAR(env->getMaxY(), 33, 1e-6);
}

TEST(BBOX_READER, CASE4) {
  std::string input = "BBOX (0,0,0,0)";
  auto env = cql2cpp::BBoxReader().read(input);
  EXPECT_NEAR(env->getMinX(), 0, 1e-6);
  EXPECT_NEAR(env->getMinY(), 0, 1e-6);
  EXPECT_NEAR(env->getMaxX(), 0, 1e-6);
  EXPECT_NEAR(env->getMaxY(), 0, 1e-6);
}

TEST(BBOX_READER, CASE5) {
  std::string input = "BBOX (0 0 0 0)";
  auto env = cql2cpp::BBoxReader().read(input);
  EXPECT_EQ(env, nullptr);
}

TEST(BBOX_READER, CASE6) {
  std::string input = "BBOX (0,0,0,0 ";
  auto env = cql2cpp::BBoxReader().read(input);
  EXPECT_EQ(env, nullptr);
}
