/// \copyright This file is under GPL 3 license. Please read the \p LICENSE file
/// at the root of \p tenacitas directory

/// \author Rodrigo Canellas - rodrigo.canellas at gmail.com

#include <cstdint>
#include <string>

#include <tenacitas.lib/container/matrix.h>
#include <tenacitas.lib/program/options.h>
#include <tenacitas.lib/log/cerr.h>
#include <tenacitas.lib/generic/fmt.h>

using namespace tenacitas::lib;

struct matrix_000 {
  static std::string desc() {
    return "simple square container::matrix creation and element access";
  }

  bool operator()(const program::options &) {
    log::cerr _log;
    container::matrix<uint16_t, char> _matrix(3, 3, 'z');

    _log.tst(generic::fmt(_matrix));

    if (_matrix(1, 2) != 'z') {
      _log.err(generic::fmt("container::matrix(1,2) should be 'z', but it is '",
                        _matrix(1, 2), '\''));
      return false;
    }
    _log.tst(generic::fmt("container::matrix(1,2) is '", _matrix(1, 2), '\''));
    return true;
  }
};

struct matrix_001 {
  static std::string desc() {
    return "create square container::matrix, modify and element access";
  }

  bool operator()(const program::options &) {
    log::cerr _log;
    container::matrix<uint16_t, char> _matrix(3, 3, 'z');

    _log.tst(generic::fmt(_matrix));

    if (_matrix(1, 2) != 'z') {
      _log.err(generic::fmt("container::matrix(1,2) should be 'z', but it is '",
                        _matrix(1, 2), '\''));
      return false;
    }

    _matrix(1, 2) = 'e';

    if (_matrix(1, 2) != 'e') {
      _log.err(generic::fmt("container::matrix(1,2) should be 'e', but it is '",
                        _matrix(1, 2), '\''));
      return false;
    }

    _log.tst(generic::fmt(_matrix));

    _log.tst(generic::fmt("container::matrix(1,2) is '", _matrix(1, 2), '\''));
    return true;
  }
};

struct matrix_002 {
  static std::string desc() {
    return "create non square container::matrix creation, fill it, and element "
           "access";
  }

  bool operator()(const program::options &) {
    log::cerr _log;
    container::matrix<uint16_t, int16_t> _matrix(3, 4, -1);

    _log.tst(generic::fmt(_matrix));

    if (_matrix(1, 2) != -1) {
      _log.err(generic::fmt("container::matrix(1,2) should be -1, but it is '",
                        _matrix(1, 2), '\''));
      return false;
    }
    _log.tst(generic::fmt("container::matrix(1,2) is '", _matrix(1, 2), '\''));

    uint16_t _i = 0;
    for (uint16_t _row = 0; _row < _matrix.get_num_rows(); ++_row) {
      for (uint16_t _col = 0; _col < _matrix.get_num_cols(); ++_col) {
        _log.tst(generic::fmt('[', _row, ',', _col, "] = ", _i));
        _matrix(_row, _col) = _i;
        ++_i;
      }
    }

    _log.tst(generic::fmt(_matrix));

    if (_matrix(1, 2) != 6) {
      _log.err(generic::fmt("container::matrix(1,2) should be 6, but it is '",
                        _matrix(1, 2), '\''));
      return false;
    }

    _log.tst(generic::fmt("container::matrix(1,2) is '", _matrix(1, 2), '\''));

    return true;
  }
};

struct matrix_003 {
  static std::string desc() { return "test filling horizontally"; }

  bool operator()(const program::options &) {
    log::cerr _log;
    container::matrix<uint16_t, char> _m(11, 8, '-');
    uint16_t _row{0};
    uint16_t _col{0};
    uint16_t _count{0};
    _log.tst(generic::fmt(_m));
    std::string _s0{"open"};
    for (char _c : _s0) {
      _m(_row, _col + _count++) = _c;
    }
    _log.tst(generic::fmt(_m));

    return ((_m(0, 0) == 'o') && (_m(0, 1) == 'p') && (_m(0, 2) == 'e') &&
            (_m(0, 3) == 'n'));
  }
};

struct matrix_004 {
  static std::string desc() { return "test filling vertically"; }

  bool operator()(const program::options &) {
    log::cerr _log;
    container::matrix<uint16_t, char> _m(11, 8, '-');
    uint16_t _row{0};
    uint16_t _col{0};
    uint16_t _count{0};
    _log.tst(generic::fmt(_m));
    std::string _s0{"open"};
    for (char _c : _s0) {
      _m(_row + _count++, _col) = _c;
    }
    _log.tst(generic::fmt(_m));

    return ((_m(0, 0) == 'o') && (_m(1, 0) == 'p') && (_m(2, 0) == 'e') &&
            (_m(3, 0) == 'n'));
  }
};
