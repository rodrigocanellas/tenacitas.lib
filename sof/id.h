/// \copyright This file is under GPL 3 license. Please read the \p LICENSE file
/// at the root of \p tenacitas directory

/// \author Rodrigo Canellas - rodrigo.canellas at gmail.com

#ifndef TENACITAS_LIB_SOF_ID_H
#define TENACITAS_LIB_SOF_ID_H

#include <chrono>
#include <cstddef>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>

using namespace std::chrono_literals;

namespace tenacitas::lib::sof {

/// \brief creation of a unique identifier
struct id {
  /// \brief Identifier based on a number
  template <typename t_number> inline id(t_number p_value) : m_value(p_value) {}

  /// \brief Identifier based on a pointer value
  template <typename t_this>
  id(t_this *p_this) : m_value(reinterpret_cast<decltype(m_value)>(p_this)) {}

  /// \brief Identifier self generated
  id() {
    std::this_thread::sleep_for(5ns);
    m_value = static_cast<decltype(m_value)>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
  }

  id(const id &) = default;
  id(id &&) = default;
  id &operator=(const id &) = default;
  id &operator=(id &&) = default;

  /// \brief
  inline constexpr bool operator==(const id &p_id) const {
    return m_value == p_id.m_value;
  }

  /// \brief
  inline constexpr bool operator!=(const id &p_id) const {
    return m_value != p_id.m_value;
  }

  /// \brief
  inline constexpr bool operator>(const id &p_id) const {
    return m_value > p_id.m_value;
  }

  /// \brief
  inline constexpr bool operator<(const id &p_id) const {
    return m_value < p_id.m_value;
  }

  /// \brief
  inline constexpr bool operator>=(const id &p_id) const {
    return m_value >= p_id.m_value;
  }

  /// \brief
  inline constexpr bool operator<=(const id &p_id) const {
    return m_value <= p_id.m_value;
  }

  /// \brief Outputs the last 4 digits of the identifier
  friend std::ostream &operator<<(std::ostream &p_out, const id &p_id) {
    p_out << p_id.string();
    return p_out;
  }

  /// \brief Retrieves the last 4 digits of the identifier
  inline operator std::string() const { return string(); }

  inline constexpr operator size_t() const { return m_value; }

  /// \brief Returns the value of the identifier as a string
  inline std::string string() const { return {std::to_string(m_value)}; }

private:
  size_t m_value{0};
};

} // namespace tenacitas::lib::sof

#endif
