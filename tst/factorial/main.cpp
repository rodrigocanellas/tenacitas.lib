/// \copyright This file is under GPL 3 license. Please read the \p LICENSE file
/// at the root of \p tenacitas directory

/// \author Rodrigo Canellas - rodrigo.canellas at gmail.com

#include <tenacitas.lib/log/log.h>
#include <tenacitas.lib/sof/program_options.h>
#include <tenacitas.lib/sof/tester.h>
#include <tenacitas.lib/mat/factorial.h>

using namespace tenacitas::lib;

struct test0000 {
  static std::string desc() { return "factorial of 0"; }
  bool operator()(const sof::program_options &) {
    auto _maybe{mat::factorial<uint16_t>(0)};
    if (!_maybe) {
      TNCT_LOG_ERR("failed to calculate the factorial of 0");
      return false;
    }
    auto _value{_maybe.value()};
    if (_value != 1) {
      TNCT_LOG_ERR("factorial of 0 should be 1, but calculated ", _value);
      return false;
    }
    return true;
  }
};

struct test0001 {
  static std::string desc() { return "factorial of 5"; }
  bool operator()(const sof::program_options &) {
    auto _maybe{mat::factorial<uint16_t>(5)};
    if (!_maybe) {
      TNCT_LOG_ERR("failed to calculate the factorial of 5");
      return false;
    }
    auto _value{_maybe.value()};
    if (_value != 120) {
      TNCT_LOG_ERR("factorial of 5 should be 120, but calculated ", _value);
      return false;
    }
    TNCT_LOG_INF("factorial of 5 is ", _value);
    return true;
  }
};

int main(int argc, char **argv) {

  log::set_debug_level();
  sof::tester _tester(argc, argv);
  run_test(_tester, test0000);
  run_test(_tester, test0001);
}
