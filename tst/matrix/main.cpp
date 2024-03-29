/// \copyright This file is under GPL 3 license. Please read the \p LICENSE file
/// at the root of \p tenacitas directory

/// \author Rodrigo Canellas - rodrigo.canellas at gmail.com

#include <cstdint>
#include <string>

#include <tenacitas.lib/con/matrix.h>
#include <tenacitas.lib/log/log.h>
#include <tenacitas.lib/sof/program_options.h>
#include <tenacitas.lib/sof/tester.h>

using namespace tenacitas::lib;

struct test_000 {
  static std::string desc() {
    return "simple square con::matrix creation and element access";
  }

  bool operator()(const sof::program_options &) {

    con::matrix<uint16_t, char> _matrix(3, 3, 'z');

    TNCT_LOG_TST(_matrix);

    if (_matrix(1, 2) != 'z') {
      TNCT_LOG_ERR("con::matrix(1,2) should be 'z', but it is '", _matrix(1, 2),
                   '\'');
      return false;
    }
    TNCT_LOG_TST("con::matrix(1,2) is '", _matrix(1, 2), '\'');
    return true;
  }
};

struct test_001 {
  static std::string desc() {
    return "create square con::matrix, modify and element access";
  }

  bool operator()(const sof::program_options &) {

    con::matrix<uint16_t, char> _matrix(3, 3, 'z');

    TNCT_LOG_TST(_matrix);

    if (_matrix(1, 2) != 'z') {
      TNCT_LOG_ERR("con::matrix(1,2) should be 'z', but it is '", _matrix(1, 2),
                   '\'');
      return false;
    }

    _matrix(1, 2) = 'e';

    if (_matrix(1, 2) != 'e') {
      TNCT_LOG_ERR("con::matrix(1,2) should be 'e', but it is '", _matrix(1, 2),
                   '\'');
      return false;
    }

    TNCT_LOG_TST(_matrix);

    TNCT_LOG_TST("con::matrix(1,2) is '", _matrix(1, 2), '\'');
    return true;
  }
};

struct test_002 {
  static std::string desc() {
    return "create non square con::matrix creation, fill it, and element "
           "access";
  }

  bool operator()(const sof::program_options &) {

    con::matrix<uint16_t, int16_t> _matrix(3, 4, -1);

    TNCT_LOG_TST(_matrix);

    if (_matrix(1, 2) != -1) {
      TNCT_LOG_ERR("con::matrix(1,2) should be -1, but it is '", _matrix(1, 2),
                   '\'');
      return false;
    }
    TNCT_LOG_TST("con::matrix(1,2) is '", _matrix(1, 2), '\'');

    uint16_t _i = 0;
    for (uint16_t _row = 0; _row < _matrix.get_num_rows(); ++_row) {
      for (uint16_t _col = 0; _col < _matrix.get_num_cols(); ++_col) {
        TNCT_LOG_TST('[', _row, ',', _col, "] = ", _i);
        _matrix(_row, _col) = _i;
        ++_i;
      }
    }

    TNCT_LOG_TST(_matrix);

    if (_matrix(1, 2) != 6) {
      TNCT_LOG_ERR("con::matrix(1,2) should be 6, but it is '", _matrix(1, 2),
                   '\'');
      return false;
    }

    TNCT_LOG_TST("con::matrix(1,2) is '", _matrix(1, 2), '\'');

    return true;
  }
};

struct test_003 {
  static std::string desc() { return "test filling horizontally"; }

  bool operator()(const sof::program_options &) {

    con::matrix<uint16_t, char> _m(11, 8, '-');
    uint16_t _row{0};
    uint16_t _col{0};
    uint16_t _count{0};
    TNCT_LOG_TST(_m);
    std::string _s0{"open"};
    for (char _c : _s0) {
      _m(_row, _col + _count++) = _c;
    }
    TNCT_LOG_TST(_m);

    return ((_m(0, 0) == 'o') && (_m(0, 1) == 'p') && (_m(0, 2) == 'e') &&
            (_m(0, 3) == 'n'));
  }
};

struct test_004 {
  static std::string desc() { return "test filling vertically"; }

  bool operator()(const sof::program_options &) {

    con::matrix<uint16_t, char> _m(11, 8, '-');
    uint16_t _row{0};
    uint16_t _col{0};
    uint16_t _count{0};
    TNCT_LOG_TST(_m);
    std::string _s0{"open"};
    for (char _c : _s0) {
      _m(_row + _count++, _col) = _c;
    }
    TNCT_LOG_TST(_m);

    return ((_m(0, 0) == 'o') && (_m(1, 0) == 'p') && (_m(2, 0) == 'e') &&
            (_m(3, 0) == 'n'));
  }
};

int main(int argc, char **argv) {

  sof::tester _tester(argc, argv);
  run_test(_tester, test_000);
  run_test(_tester, test_001);
  run_test(_tester, test_002);
  run_test(_tester, test_003);
  run_test(_tester, test_004);
}
