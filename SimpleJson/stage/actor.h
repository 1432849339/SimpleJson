/* =====================================================================================
 *         Author:  Zhang Wen(zhangwen@szkingdom.com)
 *        Created:  2014/4/30 17:49:33
 *    Description:  
 * =====================================================================================
 */

#ifndef  ISON_BASE_ACTOR_H_
#define  ISON_BASE_ACTOR_H_

#include <string>
#include <memory>
#include "isonbase.h"

namespace ison {
namespace base {

class ActorContext;
class Event;
class Stage;
class Message;

class ISONBASE_API Actor {
public:
  Actor(const std::string& id);
  virtual ~Actor();

  virtual int OnStart(ActorContext&) { return 0; }
  virtual int OnStop() { return 0; }
  virtual int OnEvent(Event&) = 0;

  int Request(const std::string& connect_id, const std::string& receiver, const std::string& message);
  int ReplyTo(Message& hops, const std::string& receiver, const std::string& message);

  int Publish(const std::string& pub_id, const std::string& topic, const std::string& message);
  int Subscribe(const std::string& sub_id, const std::string& topic);
  int UnSubscribe(const std::string& sub_id, const std::string& topic);

  int AddTimer(int delay_ms, int times = 0);
  int EndTimer(int timer_id);

  std::string id() { return id_; } 

  bool is_temporary() { return is_temporary_; }
  void set_temporary(bool is_temporary) { is_temporary_ = is_temporary; }

  Stage* stage() { return stage_; }
  void set_stage(Stage* stage) { stage_ = stage; }

private:
  const std::string id_;
  bool is_temporary_;
  Stage* stage_;
};
DEFINE_SHARED_PTR(Actor);

} //namespace base
} //namespace ison

#endif   //ACTOR_H_
