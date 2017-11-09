/* =====================================================================================
 *         Author:  Zhang Wen(zhangwen@szkingdom.com)
 *        Created:  2014-6-6 13:13
 *    Description:  
 * =====================================================================================
 */

#ifndef ISON_BASE_LOOP_H_
#define ISON_BASE_LOOP_H_

#include <functional>
#include "czmq_header.h"
#include "isonbase.h"

namespace ison {
namespace base {



//event-driven reactor
class ISONBASE_API Loop {
public:
  //  Callback function for reactor events
  typedef int (*Callback)(void* loop, zmq_pollitem_t* item, void *arg);
  // Callback for reactor timer events
  typedef int(*TimerCallback)(void* loop, int timer_id, void *arg);

  //  Create a new zloop reactor
  Loop();

  //  Destroy a reactor
  ~Loop();

  //  Register pollitem with the reactor. When the pollitem is ready, will call
  //  the handler, passing the arg. Returns 0 if OK, -1 if there was an error.
  //  If you register the pollitem more than once, each instance will invoke its
  //  corresponding handler.
  int AddPoller(zmq_pollitem_t& item, Callback handler, void *arg);

  //  Cancel a pollitem from the reactor, specified by socket or FD. If both
  //  are specified, uses only socket. If multiple poll items exist for same
  //  socket/FD, cancels ALL of them.
  void EndPoller(zmq_pollitem_t& item);

  //  Configure a registered pollitem to ignore errors. If you do not set this,
  //  then pollitems that have errors are removed from the reactor silently.
  void SetTolerant(zmq_pollitem_t& item);

  //  Register a timer that expires after some delay and repeats some number of
  //  times. At each expiry, will call the handler, passing the arg. To run a
  //  timer forever, use 0 times. Returns a timer_id that is used to cancel the
  //  timer in the future. Returns -1 if there was an error.
  int AddTimer(size_t delay, size_t times, TimerCallback handler, void *arg);

  //  Cancel a specific timer identified by a specific timer_id (as returned by
  //  zloop_timer).
  int EndTimer(int timer_id);

  //  Set verbose tracing of reactor on/off
 void SetVerbose(bool verbose);

  //  Start the reactor. Takes control of the thread and returns when the ØMQ
  //  context is terminated or the process is interrupted, or any event handler
  //  returns -1. Event handlers may register new sockets and timers, and
  //  cancel sockets. Returns 0 if interrupted, -1 if cancelled by a handler.
  int Start();

private:
  DISALLOW_COPY_AND_ASSIGN(Loop);
  zloop_t* self_;
};
DEFINE_SHARED_PTR(Loop);

} //namespace base
} //namespace ison
#endif // ISON_BASE_LOOP_H_