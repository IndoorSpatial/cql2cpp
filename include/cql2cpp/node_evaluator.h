/*
 * File Name: node_evaluator.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/19
 *
 */

#pragma once

#include "evaluator.h"

namespace cql2cpp {

const std::map<Operator, NodeEval> node_evals = {
    {NullOp,
     [](auto n, auto vs, auto ds) -> ValueT {
       return n->value();
     }},
    {And,
     [](auto n, auto vs, auto ds) -> ValueT {
       return std::get<bool>(vs.at(0)) and std::get<bool>(vs.at(1));
     }},
    {Or,
     [](auto n, auto vs, auto ds) -> ValueT {
       return std::get<bool>(vs.at(0)) or std::get<bool>(vs.at(1));
     }},
    {Not,
     [](auto n, auto vs, auto ds) -> ValueT {
       return not std::get<bool>(vs.at(0));
     }},
};

}  // namespace cql2cpp

