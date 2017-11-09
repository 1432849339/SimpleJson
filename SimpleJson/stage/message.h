/* =====================================================================================
 *         Author:  Zhang Wen(zhangwen@szkingdom.com)
 *        Created:  2014/5/4 17:55:40
 *    Description:  
 * =====================================================================================
 */

#ifndef  MESSAGE_H_
#define  MESSAGE_H_

#include <string>
#include "czmq_header.h"
#include "isonbase.h"
#include "owership.h"

namespace ison {
namespace base {

//working with single message frames
class ISONBASE_API Frame : public OwnerShip {
public:

  //  Create an empty (zero-sized) frame
  Frame();

  //  Create a new frame with optional size, and optional data
  Frame(const void* data, size_t size);

  // Create a new frame from zframe_t
  Frame(zframe_t* frame, bool is_owner = false);

  //copy contructor
  Frame(const Frame&);

  //  Destroy a frame
  ~Frame();

  //operator assign
  Frame& operator = (const Frame&);

  //clone a frame
  void Clone(const Frame&);

  //  Destroy a frame
  void Destroy();

  //  Receive frame from socket, returns zframe_t object or NULL if the recv
  //  was interrupted. Does a blocking recv, if you want to not block then use
  //  zframe_recv_nowait().
  //  Receive a new frame off the socket. Returns newly allocated frame, or
  //  NULL if there was no input waiting, or if the read was interrupted.
  int Recv(void* source, bool no_wait = false);

  // Send a frame to a socket, destroy frame after sending.
  // Return -1 on error, 0 on success.
  int Send(void* dest, int flag);//FIXME(zhangwen): 

  //  Return number of bytes in frame data
  size_t Size() const;

  //  Return address of frame data
  uint8_t* Data() const;

  //  Create a new frame that duplicates an existing frame
  Frame* Dup() const;

  //  Return frame data encoded as printable hex string
  char* StrHex() const;
  
  //  Return frame data copied into freshly allocated string
  char* StrDup() const;

  //  Return TRUE if frame body is equal to string, excluding terminator
  bool StrEqual(const char* str) const;

  //  Return frame MORE indicator (1 or 0), set when reading frame from socket
  //  or by the zframe_set_more() method
  int More() const;
  //  Set frame MORE indicator (1 or 0). Note this is NOT used when sending
  //  frame to socket, you have to specify flag explicitly.
  void SetMore(int more);

  //  Return TRUE if two frames have identical size and data
  //  If either frame is NULL, equality is always false.
  bool Equal(Frame& other) const;

  //   Print contents of the frame to FILE stream.
  void Print(const char *prefix, FILE *file = stdout) const;
  //  Set new contents for frame
  void Reset(const void* data, size_t size);

  std::string ToString() const;

  zframe_t* self() const { return self_; }

private:
  zframe_t* self_;
};

//working with multipart messages
class ISONBASE_API Message : OwnerShip {
public:
  //  Create a new empty message object
  Message();

  // Create a new message object from zmsg_t
  Message(zmsg_t* msg, bool is_owner = false);

  //copy contructor
  Message(const Message&);

  //  Destroy a message object and all frames it contains
  virtual ~Message();
  
  //operator assign
  Message& operator = (const Message&);

  //clone a frame
  void Clone(const Message&);

  //  Destroy a message object and all frames it contains
   void Destroy();

  //  Receive message from socket, returns zmsg_t object or NULL if the recv
  //  was interrupted. Does a blocking recv, if you want to not block then use
  //  the zloop class or zmsg_recv_nowait() or zmq_poll to check for socket input before receiving.
  //  Receive message from socket, returns zmsg_t object, or NULL either if there was
  //  no input waiting, or the recv was interrupted.
  int Recv(void *socket, bool no_wait = false);

  //  Send message to socket, destroy after sending. If the message has no
  //  frames, sends nothing but destroys the message anyhow. Safe to call
  //  if zmsg is null.
  int Send(void *socket, bool no_wait = false);

  //  Return size of message, i.e. number of frames (0 or more).
  size_t Size() const;

  //  Return total size of all frames in message.
  size_t ContentSize() const;

  //  Push frame to the front of the message, i.e. before all other frames.
  //  Message takes ownership of frame, will destroy it when message is sent.
  //  Returns 0 on success, -1 on error. Deprecates zmsg_push, which did not
  //  nullify the caller's frame reference.
  int Prepend(Frame* frame);

  //  Add frame to the end of the message, i.e. after all other frames.
  //  Message takes ownership of frame, will destroy it when message is sent.
  //  Returns 0 on success. Deprecates zmsg_add, which did not nullify the
  //  caller's frame reference.
  int Append(Frame* frame);

  //  Remove first frame from message, if any. Returns frame, or NULL. Caller
  //  now owns frame and must destroy it when finished with it.
  Frame* Pop();

  //  Push block of memory to front of message, as a new frame.
  //  Returns 0 on success, -1 on error.
  int PushMem(const void *src, size_t size);

  //  Add block of memory to the end of the message, as a new frame.
  //  Returns 0 on success, -1 on error.
  int AddMem(const void *src, size_t size);

  //  Push string as new frame to front of message.
  //  Returns 0 on success, -1 on error.
  int PushStr(const char *string);

  //  Push string as new frame to end of message.
  //  Returns 0 on success, -1 on error.
  int AddStr(const char *string);

  //  Push formatted string as new frame to front of message.
  //  Returns 0 on success, -1 on error.
  int PushStrf(const char *format, ...);

  //  Push formatted string as new frame to end of message.
  //  Returns 0 on success, -1 on error.
  int AddStrf(const char *format, ...);

  //  Pop frame off front of message, return as fresh string. If there were
  //  no more frames in the message, returns NULL.
  char * PopStr();

  //  Pop frame off front of message, caller now owns frame
  //  If next frame is empty, pops and destroys that empty frame.
  Frame* Unwrap();

  //  Remove specified frame from list, if present. Does not destroy frame.
  void Remove(Frame& frame);

  //  Set cursor to first frame in message. Returns frame, or NULL, if the
  //  message is empty. Use this to navigate the frames as a list.
  Frame* First() const;

  //  Return the next frame. If there are no more frames, returns NULL. To move
  //  to the first frame call zmsg_first(). Advances the cursor.
  Frame* Next() const;

  //  Return the last frame. If there are no frames, returns NULL.
  Frame* Last() const;

  //  Save message to an open file, return 0 if OK, else -1. The message is
  //  saved as a series of frames, each with length and data. Note that the
  //  file is NOT guaranteed to be portable between operating systems, not
  //  versions of CZMQ. The file format is at present undocumented and liable
  //  to arbitrary change.
  int Save(FILE *file) const;

  //  Load/append an open file into message, create new message if
  //  null message provided. Returns NULL if the message could not
  //  be loaded.
  int Load(FILE *file);

  //  Serialize multipart message to a single buffer. Use this method to send
  //  structured messages across transports that do not support multipart data.
  //  Allocates and returns a new buffer containing the serialized message.
  //  To decode a serialized message buffer, use zmsg_decode ().
  size_t Encode(uint8_t** buffer) const;

  //  Decodes a serialized message buffer created by zmsg_encode () and returns
  //  a new zmsg_t object. Returns NULL if the buffer was badly formatted or
  //  there was insufficient memory to work.
  int Decode(uint8_t* buffer, size_t buffer_size);

  //  Create copy of message, as new message object. Returns a fresh zmsg_t
  //  object, or NULL if there was not enough heap memory.
  Message* Dup() const;

  //  Print message to open stream
  //  Truncates to first 10 frames, for readability.
  void Print(FILE *file = stdout) const;

  std::string ToString() const;

  zmsg_t* self() const { return self_; }

private:
  zmsg_t* self_;
};



} //namespace base
} //namespace ison

#endif   //MESSAGE_H_
