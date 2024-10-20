/// \copyright This file is under GPL 3 license. Please read the \p LICENSE file
/// at the root of \p tenacitas directory

/// \author Rodrigo Caellas - rodrigo.caellas at gmail.com

#ifndef TENACITAS_LIB_PAIR_TEST_PAIR_OUTPUT_TEST_H
#define TENACITAS_LIB_PAIR_TEST_PAIR_OUTPUT_TEST_H

#include <iostream>
#include <utility>

#include <tenacitas.lib/pair/pair_output.h>
#include <tenacitas.lib/program/options.h>

using namespace tenacitas::lib;

namespace tenacitas::lib::pair::test {

struct pair_output_000 {
  static std ::string desc() { return "Simple pair output"; }

  bool operator()(const program::options &) {
    std::pair<int, char> _pair{-8, 'H'};

    std::cerr << "pair = " << _pair << std::endl;

    return true;
  }
};

struct pair_output_001 {
  static std ::string desc() { return "Prints a pair with a pair inside"; }

  bool operator()(const program::options &) {
    auto _pair{std::make_pair(-8, std::make_pair(0.3331, -99))};

    std::cerr << "pair = " << _pair << std::endl;

    return true;
  }
};

} // namespace tenacitas::lib::pair::test

#endif
