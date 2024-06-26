/// \copyright This file is under GPL 3 license. Please read the \p LICENSE file
/// at the root of \p tenacitas directory

/// \author Rodrigo Canellas - rodrigo.canellas at gmail.com

#ifndef TENACITAS_LIB_CONTAINER_CIRCULAR_QUEUE_H
#define TENACITAS_LIB_CONTAINER_CIRCULAR_QUEUE_H

#include <cstddef>
#include <mutex>
#include <optional>
#include <vector>

#include <tenacitas.lib/generic/fmt.h>
#include <tenacitas.lib/log/logger.h>
#include <tenacitas.lib/traits/i_queue.h>
#include <tenacitas.lib/traits/logger.h>

namespace tenacitas::lib::container {

/// \brief Implements a circular queue which size is increased if it becomes
/// full
///
/// The purpose is to avoid unnecessary memory allocations to create nodes in
/// the queue by reusing nodes which data have been read
///
/// \tparam t_data defines the types of the data contained in the queue
template <traits::logger t_logger, typename t_data>
requires std::move_constructible<t_data> && std::copy_constructible<t_data>
class circular_queue : public traits::i_queue<t_logger, t_data> {
public:
  circular_queue() = delete;

  circular_queue(std::string_view p_id, t_logger &p_logger,
                 size_t p_initial_size = 100)
      : traits::i_queue<t_logger, t_data>(p_id, p_logger),
        m_initial_size(p_initial_size == 0 ? 100 : p_initial_size),
        m_incremental_size(((m_initial_size / 2) == 0 ? m_initial_size
                                                      : (m_initial_size / 2))),
        m_vector(m_initial_size, t_data()), m_head(0), m_tail(0) {

    TNCT_LOG_TRA(this->m_logger, "creating - ", brief_report());
  }

  ~circular_queue() override {}

  circular_queue(const circular_queue &p_queue)
      : traits::i_queue<t_logger, t_data>(p_queue),
        m_initial_size(p_queue.m_inital_size),
        m_incremental_size(p_queue.m_incremental_size),
        m_vector(p_queue.m_vector), m_head(p_queue.m_head),
        m_tail(p_queue.m_tail), m_occupied(p_queue.m_occupied) {}

  circular_queue(circular_queue &&p_queue)
      : traits::i_queue<t_logger, t_data>(std::move(p_queue)),
        m_initial_size(p_queue.m_initial_size),
        m_incremental_size(p_queue.m_incremental_size),
        m_vector(std::move(p_queue.m_vector)),
        m_head(std::move(p_queue.m_head)), m_tail(std::move(p_queue.m_tail)),
        m_occupied(p_queue.m_occupied) {}

  circular_queue &operator=(const circular_queue &p_queue) {
    if (this != &p_queue) {
      std::lock_guard<std::mutex> _lock(m_mutex);
      traits::i_queue<t_logger, t_data>::operator=(p_queue);
      m_initial_size = p_queue.m_inital_size;
      m_incremental_size = p_queue.m_incremental_size;
      m_vector = p_queue.m_vector;
      m_head = p_queue.m_head;
      m_tail = p_queue.m_tail;
      m_occupied = p_queue.m_occupied;
    }
    return *this;
  }

  circular_queue &operator=(circular_queue &&p_queue) {
    if (this != &p_queue) {
      traits::i_queue<t_logger, t_data>::operator=(std::move(p_queue));
      m_initial_size = p_queue.m_inital_size;
      m_incremental_size = p_queue.m_incremental_size;
      m_vector = std::move(p_queue.m_vector);
      m_head = p_queue.m_head;
      m_tail = p_queue.m_tail;
      m_occupied = p_queue.m_occupied;
    }
    return *this;
  }

  std::string full_report() {
    std::stringstream _out;

    if (occupied() != 0) {
      _out << '\n';
    }
    _out << brief_report();
    if (occupied() != 0) {
      _out << '\n';

      auto _last(m_vector.size() - 1);

      for (decltype(_last) _idx = 0; _idx < (_last - 1); ++_idx) {
        _out << "queue[" << _idx << "] = " << m_vector[_idx] << '\n';
      }
      _out << "queue[" << _last << "] = " << m_vector[_last];
    }
    return _out.str();
  }

  std::string brief_report() {
    std::stringstream _out;
    _out << "id " << this->m_id << ", head = " << m_head
         << ", tail = " << m_tail << ", occupied = " << occupied()
         << ", capacity = " << capacity()
         << ", increment = " << m_incremental_size;
    return _out.str();
  }

  void push(t_data &&p_data) override {
    std::lock_guard<std::mutex> _lock(m_mutex);

    TNCT_LOG_TRA(this->m_logger, "push - entering ", brief_report());

    if (full()) {
      enlarge();
    }

    m_vector[m_head] = std::move(p_data);

    if (m_head == (m_vector.size() - 1)) {
      m_head = 0;
    } else {
      ++m_head;
    }
    ++m_occupied;

    TNCT_LOG_TRA(this->m_logger, "push - leaving: ", brief_report());
  }

  void push(const t_data &p_data) override {
    std::lock_guard<std::mutex> _lock(m_mutex);

    TNCT_LOG_TRA(this->m_logger, "push - entering: ", brief_report());

    if (full()) {
      enlarge();
    }

    m_vector[m_head] = p_data;

    if (m_head == (m_vector.size() - 1)) {
      m_head = 0;
    } else {
      ++m_head;
    }
    ++m_occupied;

    TNCT_LOG_TRA(this->m_logger, "push - leaving: ", brief_report());
  }

  std::optional<t_data> pop() override {
    std::lock_guard<std::mutex> _lock(m_mutex);

    TNCT_LOG_TRA(this->m_logger, "pop - entering: ", brief_report());

    if (empty()) {
      return std::nullopt;
    }

    t_data _data(m_vector[m_tail]);
    ++m_tail;

    if (m_tail == m_vector.size()) {
      m_tail = 0;
    }

    --m_occupied;

    TNCT_LOG_TRA(this->m_logger, "pop - leaving: ", brief_report());

    return {_data};
  }

  constexpr bool full() const override { return m_occupied == m_vector.size(); }

  constexpr bool empty() const override { return m_occupied == 0; }

  constexpr size_t capacity() const override { return m_vector.size(); }

  constexpr size_t occupied() const override { return m_occupied; }

  const std::string &id() const override { return this->m_id; }

  void clear() override {
    std::lock_guard<std::mutex> _lock(m_mutex);
    m_head = m_tail = 0;
    m_occupied = 0;
  }

  constexpr size_t head() const { return m_head; }
  constexpr size_t tail() const { return m_tail; }

  constexpr t_data operator[](size_t p_index) const {
    return m_vector[p_index];
  }

private:
  using vector = std::vector<t_data>;

protected:
  void enlarge() {
    TNCT_LOG_TRA(this->m_logger, "enlarging - entering ", full_report());

    vector _aux(m_vector.size() + m_incremental_size, t_data());

    auto _head(m_head);
    if (m_head == 0) {
      _head = m_vector.size();
    }

    std::move(&m_vector[_head], &m_vector[m_vector.size()],
              &_aux[_head + m_incremental_size]);

    std::move(&m_vector[0], &m_vector[_head], &_aux[0]);

    // m_head = _head + 1;

    // m_tail += m_incremental_size;

    if (m_head == 0) {
      m_head = m_vector.size();
    }

    if (m_tail != 0) {
      m_tail += m_incremental_size;
    }

    m_vector = std::move(_aux);
    TNCT_LOG_TRA(this->m_logger, "enlarging - leaving ", full_report());

    TNCT_LOG_INF(this->m_logger, "enlarged - ", brief_report());
  }

private:
  size_t m_initial_size{0};
  size_t m_incremental_size{0};
  vector m_vector;
  size_t m_head{0};
  size_t m_tail{0};
  size_t m_occupied{0};

  std::mutex m_mutex;
};

} // namespace tenacitas::lib::container

#endif
