/* =====================================================================================
 *         Author:  Zhang Wen(zhangwen@szkingdom.com)
 *        Created:  2014/05/05 11:39:14
 *    Description:  
 * =====================================================================================
 */

#ifndef ISON_BASE_SOCKET_H_
#define ISON_BASE_SOCKET_H_

#include "czmq_header.h"
#include "isonbase.h"
#include "owership.h"

namespace ison {
namespace base {

/*!
* \brief Socket types allowed by zmq
*
* The socket type choose at creation must be one of these types.
*
* Each is designed for a different use and has different limitations.
*/
enum SocketType {
  /*!
  * One to one - two way connection.\n
  * Connect to ::pair.\n
  * A \c pair socket has to be connected only one other pair socket and allows
  * two way communication between them.
  */
  pair = 0,

  /*!
  * One to many - fan out.\n
  * Connect to ::subscribe or ::xsubscribe.\n
  * Socket is send only.\n
  * Socket will drop messages and not block.\n
  * \c publish sockets allow sending of the same message to many subscribers
  * each subscriber can limit what is sent through the socket_option::subscribe
  * settings.
  */
  publish = 1,

  /*!
  * \note It seems doxygen can't work out which data is for the socket type and
  * which is for the socket option so both get listed for both.
  *
  * One to many - fair-queued.\n
  * Connect to ::publish or ::xpublish.\n
  * Socket is receive only.\n
  * The \c subscribe socket can connection to any number of publishers and will
  * fairly pull messages from each. The socket_option::subscribe settings can
  * be use to limit which messages are received and by default none are.
  */
  subscribe = 2,

  /*!
  * One to many - fair-queued.\n
  * Connect to ::push.\n
  * Socket is receive only.\n
  * The \c pull socket fairly pulls messages from all pushers it is connected
  * to.
  */
  pull = 7,

  /*!
  * One to many - load-balanced.\n
  * Connect to ::pull.\n
  * Socket is send only.\n
  * Socket will block if unable to send.\n
  * The \c push socket fairly distributes messages between any connected
  * puller sockets.
  */
  push = 8,

  /*!
  * One to many - fair-queued outgoing, last peer incoming.\n
  * Connect to ::reply or ::xreply.\n
  * Socket flips between send and receive only.\n
  * Socket will block if unable to send.\n
  * The \c request socket will fairly balance requests sent out to a
  * replier and then can only be used to receive until that replier
  * sends a reply.
  */
  request = 3,

  /*!
  * One to many - load-balanced incoming, last peer outgoing.\n
  * Connect to ::request or ::xrequest.\n
  * Socket flips between send and receive only.\n
  * Socket will drop messages and not block.\n
  * The \c reply socket can only receive until it pulls a message from a
  * requester at which point it can only send until the reply is sent.
  */
  reply = 4,

  /*!
  * One to many - fan out.\n
  * Connect to ::subscribe or ::xsubscribe.\n
  * Socket is send only with the exception of special subscription messages.\n
  * Socket will drop messages and not block.\n
  * \c xpublish act the same as ::publish sockets however also allow special
  * subscription messages to be received from subscribers.
  */
  xpublish = 9,

  /*!
  * One to many - fair-queued.\n
  * Connect to ::publish or ::xpublish.\n
  * Socket is receive only with the exception of special subscription messages\n
  * \c xsubscribe act the same as ::subscribe sockets however also allow special
  * subscription messages to be send to connected publishers.
  */
  xsubscribe = 10,

  /*!
  * One to many - fair-queued incoming, load-balanced outgoing.\n
  * Connect to ::reply or ::xreply.\n
  * Socket will block if unable to send.\n
  * An \c xrequest socket balances requests between repliers and pulls replies
  * back in a fair manner. Each request is expected to have exactly one reply.
  */
  xrequest = 5,

  /*!
  * One to many - fair-queued incoming, targeted outgoing.\n
  * Connect to ::request or ::xrequest.\n
  * Socket will drop messages and not block.\n
  * An \c xreply socket fairly pulls in requests from requesters and will
  * label requests so it can return replies back to the correct target.
  */
  xreply = 6,

  // To match for people who prefer the shorter versions
  pub = 1,    /*!< version of ::publish to match zmq name convention */
  sub = 2,    /*!< version of ::subscribe to match zmq name convention */
  req = 3,    /*!< version of ::request to match zmq name convention */
  rep = 4,    /*!< version of ::reply to match zmq name convention */
  xpub = 9,   /*!< version of ::xpublish to match zmq name convention */
  xsub = 10,   /*!< version of ::xsubscribe to match zmq name convention */
  xreq = 5,   /*!< version of ::xrequest to match zmq name convention */
  xrep = 6,   /*!< version of ::xreply to match zmq name convention */

  // For completion
  router = 6, /*!< \deprecated Matches zmq 2.x xrep functionality. */
  dealer = 5  /*!< \deprecated Matches zmq 2.x xreq functionality. */
};


class Context;

class ISONBASE_API Socket : public OwnerShip {
public:
  //  Callback function for zero-copy methods
  typedef void (zsocket_free_fn)(void *data, void *arg);

  //  Create a new socket within our CZMQ context, replaces zmq_socket.
  //  Use this to get automatic management of the socket at shutdown.
  //  Note: SUB sockets do not automatically subscribe to everything; you
  //  must set filters explicitly.
  Socket(Context& ctx, SocketType type);

  //copy from zsock_t
  Socket(zsock_t* sock, bool is_owner = false);

  //  Destroy a socket within our CZMQ context, replaces zmq_close.
  ~Socket();

  //  Bind a socket to a formatted endpoint. If the port is specified as
  //  '*', binds to any free port from ZSOCKET_DYNFROM to ZSOCKET_DYNTO
  //  and returns the actual port number used. Otherwise asserts that the
  //  bind succeeded with the specified port number. Always returns the
  //  port number if successful.
  int Bind(const char *format, ...);

  //  Unbind a socket from a formatted endpoint.
  //  Returns 0 if OK, -1 if the endpoint was invalid or the function
  //  isn't supported.
  int Unbind(const char *format, ...);

  //  Connect a socket to a formatted endpoint
  //  Returns 0 if OK, -1 if the endpoint was invalid.
  int Connect(const char *format, ...);

  //  Disconnect a socket from a formatted endpoint
  //  Returns 0 if OK, -1 if the endpoint was invalid or the function
  //  isn't supported.
  int Disconnect(const char *format, ...);

  //  Poll for input events on the socket. Returns TRUE if there is input
  //  ready on the socket, else FALSE.
  bool Poll(int msecs);

  //  Returns socket type as printable constant string
  const char* TypeStr();

  //  Send a signal over a socket. A signal is a zero-byte message.
  //  Signals are used primarily between threads, over pipe sockets.
  //  Returns -1 if there was an error sending the signal.
  int Signal();

  //  Wait on a signal. Use this to coordinate between threads, over
  //  pipe pairs. Returns -1 on error, 0 on success.
  int Wait();

  //  Send data over a socket as a single message frame.
  //  Returns -1 on error, 0 on success
  //  Accepts these flags: ZFRAME_MORE and ZFRAME_DONTWAIT.
  int SendMem(const void *data, size_t size, int flags = 0);

  int SetSndhwm(int n);
  int SetRcvhwm(int n);
  int SetReconnectIvl(int ivl);
  int SetReconnectIvlMax(int ivl_max);
  int SetIdentity(std::string& id);

  operator void*() { return self_; }
  
  zsock_t* self() { return self_; }

private:
  DISALLOW_COPY_AND_ASSIGN(Socket);
  Context* ctx_;
  zsock_t* self_;
};
DEFINE_SHARED_PTR(Socket);

} //namespace base
} //namespace ison

#endif 
