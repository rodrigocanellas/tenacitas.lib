/// \copyright This file is under GPL 3 license. Please read the \p LICENSE file
/// at the root of \p tenacitas directory

/// \author Rodrigo Canellas - rodrigo.canellas at gmail.com

#ifndef TENACITAS_LIB_ASYNC_INTERNAL_HANDLING_H
#define TENACITAS_LIB_ASYNC_INTERNAL_HANDLING_H

#include <condition_variable>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>
#include <typeindex>
#include <vector>

#include <tenacitas.lib/async/handling_id.h>
#include <tenacitas.lib/async/result.h>
#include <tenacitas.lib/generic/fmt.h>
#include <tenacitas.lib/log/logger.h>
#include <tenacitas.lib/traits/event.h>
#include <tenacitas.lib/traits/handler.h>
#include <tenacitas.lib/traits/logger.h>
#include <tenacitas.lib/traits/queue.h>

namespace tenacitas::lib::async::internal {

// class handling_base {
// public:
//   virtual const std::type_index &get_event_id() const = 0;

//   virtual ~handling_base() {}

//   template <traits::event t_event> result add_event(const t_event &) {
//     return async::result::UNIDENTIFIED;
//   }
// };

template <traits::logger t_logger, traits::event t_event,
          traits::queue<t_logger, t_event> t_queue, traits::handler t_handler>
class handling final /*: public handling_base*/ {
public:
  using logger = t_logger;
  using event = t_event;
  using handler = t_handler;
  using queue = t_queue;

  /// \brief Creates a handling for an event type
  ///
  /// \param p_logger
  /// \param p_handler
  handling(std::string_view p_handling_id, t_logger &p_logger,
           handler &&p_handler)
      : m_logger(p_logger), m_handling_id(p_handling_id),
        m_handler(std::move(p_handler)),
        m_queue("queue-" + m_handling_id, p_logger) {}

  handling(const handling &) = delete;

  handling(handling &&p_handling)
      : m_logger(p_handling.m_logger), m_handling_id(p_handling.m_handling_id),
        m_handler(std::move(p_handling.m_handler)),
        m_queue(p_handling.m_queue.id(), p_handling.m_logger) {
    const bool _right_handling_was_stopped{p_handling.is_stopped()};
    p_handling.stop();

    m_queued_data.store(p_handling.m_queued_data);
    m_queue = std::move(p_handling.m_queue);
    if (!_right_handling_was_stopped) {
      increment_handlers(p_handling.get_num_handlers());
    }
  }

  ~handling() {
    TNCT_LOG_TRA(m_logger, trace("entering destructor"));
    stop();
    TNCT_LOG_TRA(m_logger, trace("leaving destructor"));
  }

  handling &operator=(const handling &) = delete;
  handling &operator=(handling &&) = delete;

  const std::type_index &get_event_id() const { return m_event_id; }

  // Adds an event to the queue of events
  [[nodiscard]] result add_event(const event &p_event) {
    try {
      TNCT_LOG_TRA(m_logger, trace("adding event"));

      m_queue.push(p_event);

      std::lock_guard<std::mutex> _lock(m_data_mutex);
      m_data_cond.notify_all();

      ++m_queued_data;

      return result::OK;
    } catch (std::exception &_ex) {
      m_logger.err(
          generic::fmt("error adding event: " + std::string(_ex.what())));
    }
    return result::UNIDENTIFIED;
  }

  // Adds a bunch of event handlers
  template <std::unsigned_integral t_num_handlers>
  void increment_handlers(t_num_handlers p_num_handlers) {
    if (p_num_handlers == 0) {
      return;
    }

    if (m_stopped) {
      TNCT_LOG_TRA(m_logger,
                   trace("not adding subscriber because handling is stopped"));
      return;
    }

    std::lock_guard<std::mutex> _lock(m_add_subscriber_mutex);

    TNCT_LOG_TRA(m_logger, trace(generic::fmt("adding ", p_num_handlers,
                                              " event handlers")));

    for (decltype(p_num_handlers) _i = 0; _i < p_num_handlers; ++_i) {
      m_handling_handlers.push_back(m_handler);

      m_loops.push_back(std::thread(
          [this]() -> void { handler_loop(m_handling_handlers.size() - 1); }));
    }
  }

  // \brief Stops this handling
  void stop() {
    TNCT_LOG_TRA(m_logger, trace("entering stop()"));

    // using loop_lockers = std::vector<std::lock_guard<std::mutex>>;
    // loop_lockers _loop_lockers;
    // for (loop_mutexes::value_type &_loop_mutex : m_loop_mutexes) {
    //   _loop_lockers.push_back(std::lock_guard<std::mutex>{_loop_mutex});
    // }

    if (m_stopped) {
      TNCT_LOG_TRA(m_logger, trace("not stopping because it is stopped"));

      std::lock_guard<std::mutex> _lock(m_data_mutex);
      m_data_cond.notify_all();
      return;
    }
    TNCT_LOG_TRA(m_logger, trace("notifying loops to stop"));

    {
      std::lock_guard<std::mutex> _lock(m_data_mutex);
      m_stopped.store(true);
      m_data_cond.notify_all();
    }

    for (std::thread &_thread : m_loops) {
      if (_thread.joinable()) {
        TNCT_LOG_TRA(m_logger, trace("is joinable", _thread.get_id()));

        _thread.join();

        TNCT_LOG_TRA(m_logger, trace("joined", _thread.get_id()));

      } else {
        TNCT_LOG_TRA(m_logger, trace("is not joinable", _thread.get_id()));
      }
    }
    TNCT_LOG_TRA(m_logger, trace("leaving stop()"));
  }

  // Informs if the publishing is stopped
  constexpr bool is_stopped() const { return m_stopped; }

  [[nodiscard]] constexpr size_t get_num_handlers() const {
    return m_handling_handlers.size();
  }

  [[nodiscard]] const handling_id &get_id() const { return m_handling_id; }

  // \return Returns the amount of \p t_event objects in the queue
  [[nodiscard]] constexpr size_t get_num_events() const {
    return m_queue.occupied();
  }

  void clear() {
    std::lock_guard<std::mutex> _lock(m_data_mutex);
    m_queue.clear();
    m_data_cond.notify_all();
  }

  friend std::ostream &operator<<(std::ostream &p_out,
                                  const handling &p_handling) {
    p_out << "{name " << typeid(event).name() << ", address " << &p_handling
          << ", id " << p_handling.m_handling_id << ", queue { capacity "
          << p_handling.m_queue.capacity() << ", occupied "
          << p_handling.m_queue.occupied() << " }"
          << ", stopped? " << (p_handling.m_stopped ? 'T' : 'F') << ", #loops "
          << p_handling.m_loops.size() << '}';
    return p_out;
  }

  std::string trace(std::string_view p_text,
                    std::thread::id p_thread = std::this_thread::get_id()) {
    std::ostringstream _stream;
    _stream << *this << " - thread = " << p_thread << " - " << p_text;
    return _stream.str();
  }

private:
  // Group of async_loops that asynchronously call the handlers
  using async_loops = std::vector<std::thread>;

  // Group of handlers
  using handling_handlers = std::vector<handler>;

  using handling_handler_pos = typename handling_handlers::size_type;

private:
  // Waits for an event to be available in the event queue, then remove and call
  // the handler in \p p_handling_handler_pos in \p m_handling_handlers.
  // It exits when \p m_stop is set.
  void handler_loop(handling_handler_pos p_handling_handler_pos) {
    // auto _queue_addr{&m_queue};
    auto _loop_id{std::this_thread::get_id()};

    TNCT_LOG_TRA(m_logger, trace("starting subscriber's loop", _loop_id));

    // auto _subscriber_id{&m_handling_handlers[p_handling_handler_pos]};

    if (m_stopped) {
      TNCT_LOG_TRA(m_logger, trace("already stopped", _loop_id));
      return;
    }

    TNCT_LOG_TRA(m_logger, trace("entering loop"));

    while (true) {
      {
        TNCT_LOG_TRA(m_logger, trace("locking", _loop_id));

        std::unique_lock<std::mutex> _lock(m_data_mutex);
        m_data_cond.wait(
            _lock,
            [this, /*_subscriber_id,*/ _loop_id /*,_queue_addr*/]() -> bool {
              TNCT_LOG_TRA(m_logger, trace("entering condition", _loop_id));

              if (m_stopped) {
                TNCT_LOG_TRA(m_logger, trace("stopped", _loop_id));
                return true;
              }
              if (!m_queue.empty()) {
                TNCT_LOG_TRA(m_logger, trace("there is data", _loop_id));
                return true;
              }
              TNCT_LOG_TRA(m_logger, trace("waiting", _loop_id));
              return false;
            });
      }

      TNCT_LOG_TRA(m_logger, trace("lock released", _loop_id));

      if (m_stopped) {
        TNCT_LOG_TRA(m_logger, trace("stopped due to explicit stop", _loop_id));
        break;
      }

      TNCT_LOG_TRA(m_logger, trace("getting event from the queue", _loop_id));

      std::optional<event> _maybe{m_queue.pop()};
      if (!_maybe.has_value()) {
        TNCT_LOG_TRA(m_logger, trace("no event in queue", _loop_id));
        continue;
      }
      event _event{std::move(*_maybe)};

      TNCT_LOG_TRA(m_logger, trace("about to call subscriber", _loop_id));

      m_handling_handlers[p_handling_handler_pos](std::move(_event));

      TNCT_LOG_TRA(m_logger, trace("event handled", _loop_id));
    }

    TNCT_LOG_TRA(m_logger, trace("leaving subscriber's loop", _loop_id));
  }

  void empty_queue(const std::thread::id &p_loop_id, handler p_subscriber) {
    TNCT_LOG_TRA(m_logger, trace("entering empty_queue", p_loop_id));

    while (true) {
      std::optional<event> _maybe{m_queue.pop()};
      if (!_maybe.has_value()) {
        break;
      }

      if (m_stopped) {
        TNCT_LOG_TRA(m_logger, trace("stop", p_loop_id));
        break;
      }

      event _event{std::move(*_maybe)};

      if (this->m_stopped) {
        TNCT_LOG_TRA(m_logger, trace("stop", p_loop_id));
        break;
      }

      TNCT_LOG_TRA(m_logger, trace("calling a subscriber", p_loop_id));

      p_subscriber(std::move(_event));

      TNCT_LOG_TRA(m_logger, trace("event handled", p_loop_id));
    }
  }

private:
  logger &m_logger;

  handling_id m_handling_id;

  handler m_handler;

  queue m_queue;

  std::type_index m_event_id{std::type_index(typeid(event))};

  // Indicates if the dispatcher should continue to run
  std::atomic_bool m_stopped{false};

  // Amount of queued data
  std::atomic_size_t m_queued_data{0};

  handling_handlers m_handling_handlers;

  // Asynchronous async_loops, where events are retrieved from queue, and
  // subscribers are called
  async_loops m_loops;

  // Controls access to the \p m_loops while inserting a new subscriber
  std::mutex m_add_subscriber_mutex;

  // Controls access to inserting data
  std::mutex m_data_mutex;

  // Controls access to the data produced
  std::condition_variable m_data_cond;
};

} // namespace tenacitas::lib::async::internal

#endif
