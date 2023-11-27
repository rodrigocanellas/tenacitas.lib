#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>

#include <tenacitas.h>

// struct test0000 {
//   static std::string desc() {
//     return "underflow when multiplying to a int64_t";
//   }

//  bool operator()(tncta::program_options &) {

//    int64_t _v0{std::numeric_limits<int64_t>::min() + 10};

//    int16_t _v1{2};

//    TNCT_LOG_TST("v0 = ", _v0, " v1 = ", _v1);

//    auto _maybe{math::tncta::mul<int64_t>(_v0, _v1)};

//    if (_maybe) {
//      TNCT_LOG_ERR("it returned, ", _maybe.value(), ", but it should not");
//      return false;
//    }

//    TNCT_LOG_TST("it did not return, as it should");

//    return true;
//  }
//};

// struct test0001 {
//   static std::string desc() { return "overflow when multiplying to a
//   int64_t"; }

//  bool operator()(tncta::program_options &) {

//    int64_t _v0{std::numeric_limits<int64_t>::max() - 10};

//    int16_t _v1{2};

//    TNCT_LOG_TST("v0 = ", _v0, " v1 = ", _v1);

//    auto _maybe{math::tncta::mul<int64_t>(_v0, _v1)};

//    if (_maybe) {
//      TNCT_LOG_ERR("it returned, ", _maybe.value(), ", but it should not");
//      return false;
//    }

//    TNCT_LOG_TST("it did not return, as it should");

//    return true;
//  }
//};

// struct test0002 {
//   static std::string desc() {
//     return "overflow when multiplying to a uint64_t";
//   }

//  bool operator()(tncta::program_options &) {

//    uint64_t _v0{std::numeric_limits<uint64_t>::max() - 10};

//    int16_t _v1{2};

//    TNCT_LOG_TST("v0 = ", _v0, " v1 = ", _v1);

//    auto _maybe{math::tncta::mul<uint64_t>(_v0, _v1)};

//    if (_maybe) {
//      TNCT_LOG_ERR("it returned, ", _maybe.value(), ", but it should not");
//      return false;
//    }

//    TNCT_LOG_TST("it did not return, as it should");

//    return true;
//  }
//};

using namespace tenacitas::lib;

struct test0000 {
  static std::string desc() { return "factorial of 0"; }
  bool operator()(const tncta::program_options &) {
    auto _maybe{tncta::factorial<uint16_t>(0)};
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
  bool operator()(const tncta::program_options &) {
    auto _maybe{tncta::factorial<uint16_t>(5)};
    if (!_maybe) {
      TNCT_LOG_ERR("failed to calculate the factorial of 5");
      return false;
    }
    auto _value{_maybe.value()};
    if (_value != 120) {
      TNCT_LOG_ERR("factorial of 5 should be 1, but calculated ", _value);
      return false;
    }
    return true;
  }
};

int main(int argc, char **argv) {
  using namespace tenacitas;

  tncta::set_debug_level();
  tncta::tester _tester(argc, argv);
  run_test(_tester, test0000);
  run_test(_tester, test0001);
  //  run_test(_tester, test0002);
}