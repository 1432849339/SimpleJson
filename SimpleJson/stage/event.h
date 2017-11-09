/* =====================================================================================
 *         Author:  Zhang Wen(zhangwen@szkingdom.com)
 *        Created:  2014/6/5 14:32
 *    Description:  
 * =====================================================================================
 */

#ifndef ISON_BASE_EVENT_H_
#define ISON_BASE_EVENT_H_

#include <string>
#include "message.h"
#include "isonbase.h"

namespace ison {
namespace base {

class Socket;
class Message;
class Stage;
class IBProtocol;

enum EventType {
  kMessage = 0,
  kPublish = 1,
  kTimer = 2,
};

class ISONBASE_API Event {
public:
  explicit Event(EventType type = kMessage) 
    : type_(type), 
    stage_(NULL), 
    socket_(NULL), 
    timer_id_(0) {}

  int Reply(const std::string& message);
  int Forward(const std::string& queue, const std::string& receiver, const std::string& message);

  EventType type() { return type_; }
  void set_type(EventType type) { type_ = type; }

  Stage* stage() { return stage_; }
  void set_stage(Stage* stage) { stage_ = stage; }

  Socket* socket() { return socket_; }
  void set_socket(Socket* socket) { socket_ = socket; }

  std::string receiver() { return receiver_; }
  void set_receiver(const std::string& receiver) { receiver_ = receiver; }

  std::string sender() { return sender_; }
  void set_sender(const std::string& sender) { sender_ = sender; }

  std::string message() { return message_; }
  void set_message(const std::string& message) { message_ = message; }

  std::string topic() { return topic_; }
  void set_topic(const std::string& topic) { topic_ = topic; }

  Message hops() { return hops_; }
  void set_hops(Message hops) { hops_ = hops; }

  int timer_id() { return timer_id_; }
  void set_timer_id(int timer_id) { timer_id_ = timer_id; }

private:
  EventType type_;
  Stage* stage_;
  Socket* socket_;
  //FIXME(zhangwen): 
  std::string receiver_;
  std::string sender_;
  std::string message_;
  std::string topic_;
  Message hops_;
  int timer_id_;
};


}
}

#endif