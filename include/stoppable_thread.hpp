/* Copyright (C) 2020 Michael de Gans
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE.mit file for details.
 */

#ifndef E589C439_F4B4_4D67_9114_8A83FA001DB2
#define E589C439_F4B4_4D67_9114_8A83FA001DB2

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>

namespace nvcvcam::thread {

/** interval for sleeping (eg. while waiting) */
const uint64_t SLEEP_INTERVAL_NS = 100000;

/**
 * Thread states
 */
enum State {
  STOPPED = 0x1,
  INITIALIZING = 0x2,
  RUNNING = 0x4,
  FAILED = 0x8,
};

/**
 * Base class for threads. Derived classes need to `tick` and probably `setup`
 * and `cleanup`.
 */
class StoppableThread {
  /** set to request shutdown of the thread */
  std::atomic_bool _stopping;
  /** state of the thread */
  std::atomic<State> _state;
  /** the actual thread */
  std::unique_ptr<std::thread> _thread;

  /**
   * @brief Main thread function. Calls setup, run, cleanup.
   *
   * @return true on success
   * @return false on failure
   */
  virtual void execute();

 protected:
  /**
   * @brief setup any resources needed for tick
   *
   * @return true on success
   * @return false on failure
   */
  virtual bool setup() { return true; };
  /**
   * @brief called just after `run` sets the running state. Override if you need
   * to do something before the first iteration.
   *
   * @return true to continue
   * @return false to abort
   */
  virtual bool on_running() { return true; }
  /**
   * @brief main loop of the thread. Runs `tick` in a loop while `!_stopping`
   * and `_state` == `State::RUNNING`. Calls cleanup for you on exit.
   *
   * @return true if tick returned true
   * @return false if tick returned false or `
   */
  virtual bool run();
  /**
   * @brief an iteration of the thread loop
   *
   * @return true to continue iteration
   * @return false to stop iteration
   */
  virtual bool tick() = 0;
  /**
   * @brief cleanup any resources needed for tick
   *
   * @return true on success
   * @return false calls terminate
   */
  virtual bool cleanup() { return true; };

 public:
  StoppableThread()
      : _stopping(false), _state(State::STOPPED), _thread(nullptr){};
  virtual ~StoppableThread();

  /**
   * @brief start the thread
   *
   * @param block until ready
   * @param timeout to block for if block (default is forever)
   *
   * @return true on success
   * @return false on failure
   */
  bool start(
      bool block = true,
      std::chrono::nanoseconds timeout = std::chrono::nanoseconds::max());
  /**
   * @brief wait until a thread state
   *
   * @param state to wait until. `State` can be combined with bitwise operators.
   * @param timeout to wait for (default is forever)
   * @return true
   * @return false
   */
  bool wait(State state,
            std::chrono::nanoseconds timeout = std::chrono::nanoseconds::max());
  /**
   * @brief stop the thread. if already stopped does nothing.
   *
   * NOTE: called by destructor automatically
   *
   * @param block until ready
   * @param timeout to block for if block (default is forever)
   * @return true
   * @return false
   */
  bool stop(bool block = true,
            std::chrono::nanoseconds timeout = std::chrono::nanoseconds::max());
  /**
   * @brief query the state of the thread.
   *
   * @return true if ready to do it's thing
   * @return false if not in the `RUNNING` state
   */
  bool ready() { return _state == State::RUNNING; };
};

}  // namespace nvcvcam::thread
#endif /* E589C439_F4B4_4D67_9114_8A83FA001DB2 */
