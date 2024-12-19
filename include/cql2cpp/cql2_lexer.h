/*
 * File Name: cql2_lexer.h
 *
 * Copyright (c) 2024 IndoorSpatial
 *
 * Author: Kunlin Yu <yukunlin@syriusrobotics.com>
 * Create Date: 2024/12/19
 *
 */

#pragma once

// we include FlexLexer.h above cql2_lexer-internal.h just like the generated cc
// file by flex++. So we can include current header file in our main.cc
// https://stackoverflow.com/questions/71300080/change-yylex-in-c-flex
#include <FlexLexer.h>
#include "cql2_lexer-internal.h"
