/*
 * File Name: bbox_reader.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/27
 *
 */

#pragma once

#include <geos/geom/Envelope.h>
#include <geos/io/WKTReader.h>
#include <glog/logging.h>

#include <regex>

namespace cql2cpp {

const std::string pattern_str =
    R"(BBOX\s*[(]\s*([-+]?\d*\.?\d+)\s+([-+]?\d*\.?\d+)\s+([-+]?\d*\.?\d+)\s+([-+]?\d*\.?\d+)\s*[)])";

class BBoxReader {
 private:
 public:
  BBoxReader() {}

  geos::geom::Envelope::Ptr read(const std::string& bbox_tagged_text) const {
    std::regex pattern(pattern_str);
    std::smatch matches;
    if (std::regex_match(bbox_tagged_text, matches, pattern)) {
      if (matches.size() != 5) return nullptr;
      double minx = std::stod(matches[1]);
      double miny = std::stod(matches[2]);
      double maxx = std::stod(matches[3]);
      double maxy = std::stod(matches[4]);
      return std::make_unique<geos::geom::Envelope>(minx, maxx, miny, maxy);
    }
    return nullptr;
  }
};

}  // namespace cql2cpp
