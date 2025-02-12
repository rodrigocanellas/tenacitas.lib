/// \copyright This file is under GPL 3 license. Please read the \p LICENSE file
/// at the root of \p tenacitas directory

/// \author Rodrigo Canellas - rodrigo.canellas at gmail.com

#ifndef TNCT_TUPLE_TUPLE_OUTPUT_H
#define TNCT_TUPLE_TUPLE_OUTPUT_H

#include <iostream>
#include <sstream>
#include <tuple>

#include "tnct/traits/is_tuple.h"
#include "tnct/traits/tuple_like.h"
#include "tnct/tuple/tuple_traverse.h"

namespace tnct::tuple {

template <typename t_output_stream, traits::tuple_like t_tuple>
t_output_stream &tuple2out(t_output_stream &p_out, const t_tuple &p_tuple,
                           const char p_left_delimeter = '(',
                           const char p_right_delimeter = ')') {
  p_out << p_left_delimeter;

  auto _visit = [&]<traits::tuple_like t_tuple_lambda, size_t t_idx>(
                    const t_tuple_lambda &p_t) {
    if constexpr (traits::is_tuple_v<
                      std::tuple_element_t<t_idx, t_tuple_lambda>>) {
      tuple2out(p_out, std::get<t_idx>(p_t));
    } else {
      p_out << std::get<t_idx>(p_t);
    }
    if constexpr (t_idx < (std::tuple_size_v<t_tuple_lambda> - 1)) {
      p_out << ',';
    }
    return true;
  };

  tuple::tuple_constant_traverse<t_tuple, decltype(_visit)>(p_tuple, _visit);
  p_out << p_right_delimeter;
  return p_out;
}

} // namespace tnct::tuple

template <tnct::traits::tuple_like t_tuple>
std::ostream &operator<<(std::ostream &p_out, const t_tuple &p_tuple) {
  return tnct::tuple::tuple2out(p_out, p_tuple);
}

template <tnct::traits::tuple_like t_tuple>
std::ostringstream &operator<<(std::ostringstream &p_out,
                               const t_tuple &p_tuple) {
  return tnct::tuple::tuple2out(p_out, p_tuple);
}

#endif
