/* =====================================================================================
 *         Author:  Zhang Wen(zhangwen@szkingdom.com)
 *        Created:  2014-6-20 14:57
 *    Description:  
 * =====================================================================================
 */

#ifndef ISON_BASE_THREAD_H_
#define ISON_BASE_THREAD_H_

#include <thread>
#include <atomic>
#include <memory>
#include "isonbase.h"

namespace ison {
namespace base {

class ISONBASE_API Thread {
public:
  enum State {
    kStopped = 0,
    kRunning,
    kStoping,
  };

  Thread() : state_(kStopped) {}
  virtual ~Thread() { if (self_ && self_->joinable())  self_->join(); }

  virtual void Run() = 0;

  virtual void Start() { 
    if (!self_) {
      self_.reset(new std::thread(std::bind(&Thread::Run, this)));
      state_ = kRunning;
    }
  }
  
  virtual void Stop() {
    state_ = kStoping;
  }
  
  virtual void Join() {
    if (self_) {
      self_->join();
      Stopped();
    }
  }

  void Stopped() {
    state_ = kStopped;
  }

  bool IsRunning() {
    return state_ == kRunning;
  }

  bool IsStopping() {
    return state_ == kStoping;
  }

  bool IsStopped() {
    return state_ == kStopped;
  }

private:
  std::shared_ptr<std::thread> self_;
  std::atomic<int> state_;
};

} //namespace base
} //namespace ison

#endif // ISON_BASE_THREAD_H_