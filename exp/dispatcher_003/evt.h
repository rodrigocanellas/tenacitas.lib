#ifndef TENACITAS_TST_ASYNC_DISPATCHER_EVT_H
#define TENACITAS_TST_ASYNC_DISPATCHER_EVT_H

#include <iostream>
#include <memory>
#include <utility>

#include "typ.h"
#include <tenacitas.h>

namespace evt {

struct pressure_generated {
  pressure_generated() = default;
  pressure_generated(const pressure_generated &) = default;
  pressure_generated(pressure_generated &&) = default;
  pressure_generated &operator=(const pressure_generated &) = default;
  pressure_generated &operator=(pressure_generated &&) = default;
  explicit pressure_generated(tnctt::id p_generator,
                              typ::pressure p_value = 1.5)
      : m_generator(p_generator), m_pressure(p_value) {}

  inline bool operator==(const pressure_generated &p_pressure) const {
    return m_pressure == p_pressure.m_pressure;
  }

  inline bool operator!=(const pressure_generated &p_pressure) const {
    return m_pressure != p_pressure.m_pressure;
  }

  friend std::ostream &operator<<(std::ostream &p_out,
                                  const pressure_generated &p_pressure) {
    p_out << "'generated (" << p_pressure.m_generator << ','
          << tncta::format_number(p_pressure.m_pressure) << ")'";
    return p_out;
  }

  pressure_generated &operator++() {
    ++m_pressure;
    return *this;
  }

  pressure_generated &operator++(int) {
    m_pressure++;
    return *this;
  }

  pressure_generated &operator+=(const pressure_generated &p_pressure) {
    m_pressure += p_pressure.m_pressure;
    return *this;
  }

  pressure_generated &operator-=(const pressure_generated &p_pressure) {
    m_pressure -= p_pressure.m_pressure;
    return *this;
  }

private:
  tnctt::id m_generator;
  typ::pressure m_pressure{0};
};

struct pressure_sent {
  pressure_sent() = default;
  pressure_sent(tnctt::id p_generator) : m_generator(p_generator) {}

  friend std::ostream &operator<<(std::ostream &p_out,
                                  const pressure_sent &p_pressure_sent) {
    p_out << "'sent (" << p_pressure_sent.m_generator << ")'";
    return p_out;
  }

private:
  tnctt::id m_generator;
};

struct pressure_handled {
  pressure_handled() = default;

  pressure_handled(tnctt::queue_id p_queue, tnctt::id p_subscriber)
      : queue(p_queue), subscriber(p_subscriber) {}

  friend std::ostream &operator<<(std::ostream &p_out,
                                  const pressure_handled &p_handled) {
    p_out << "'handled (" << p_handled.queue << "," << p_handled.subscriber
          << ")'";
    return p_out;
  }

  tnctt::queue_id queue;

  tnctt::id subscriber;
};

} // namespace evt

#endif // EVT_H