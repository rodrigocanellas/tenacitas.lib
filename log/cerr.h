/// \copyright This file is under GPL 3 license. Please read the \p LICENSE file
/// at the root of \p tenacitas directory

/// \author Rodrigo Canellas - rodrigo.canellas at gmail.com

#ifndef TENACITAS_LIB_LOG_CERR_LOG_H
#define TENACITAS_LIB_LOG_CERR_LOG_H

#include <string_view>

#include <tenacitas.lib/log/internal/logger.h>

namespace tenacitas::lib::log {

struct cerr_writer {
  void operator()(std::string_view p_str) { std::cerr << p_str; }
};

using cerr = tenacitas::lib::log::internal::logger<cerr_writer>;

} // namespace tenacitas::lib::log

#endif
