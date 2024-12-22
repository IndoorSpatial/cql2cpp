/*
 * File Name: feature_source.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/19
 *
 */

#pragma once

#include "value.h"

namespace cql2cpp {

class FeatureSource {
 public:
   virtual ValueT get_property(const std::string& property_path) = 0;
   virtual ~FeatureSource() {}
};

};  // namespace cql2cpp
