/* =====================================================================================
 *         Author:  Zhang Wen(zhangwen@szkingdom.com)
 *        Created:  2014/4/30 17:55:23
 *    Description:  
 * =====================================================================================
 */

#ifndef  ISON_BASE_STAGE_H_
#define  ISON_BASE_STAGE_H_

#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include "czmq_header.h"
#include "thread.h"
#include "database_engine.h"
#include "memdb_engine.h"
#include "proxy.h"
#include "ibconfig.h"
#include "isonbase.h"

struct zmq_pollitem_t;

namespace ison {
namespace base {

class Context;
class Socket;
class Actor;
class Loop;
class Message;
DEFINE_SHARED_PTR(Socket);
DEFINE_SHARED_PTR(Actor);
DEFINE_SHARED_PTR(Loop);

class ISONBASE_API Stage : public Thread {
public:
  typedef std::vector<ActorPtr> ActorArray;
  typedef std::unordered_map<std::string, ActorPtr> ActorMap;
  typedef std::unordered_map<std::string, SocketPtr> SocketMap;
  typedef std::unordered_map<zsock_t*, std::string> SocketActorMap;
  typedef std::unordered_map<std::string, std::string> SocketEndpointMap;
  typedef std::unordered_map<std::string, ConnectionConfig> SocketOptionMap;
  typedef std::unordered_map<int, std::string> TimerActorMap;
  typedef std::set<zsock_t*> SocketSet;

  Stage(Context& ctx, const std::string& id = "", int no = 0);
  virtual ~Stage();

  //proxy for zloop
  static int SocketCallback(void* loop, zmq_pollitem_t* item, void *arg);
  static int TimerCallback(void* loop, int time_id, void *arg);

  int AddActor(ActorPtr actor);
  int RemoveActor(const std::string& id);
  
  int Bind(const std::string& endpoint);
  int Bind(ConnectionConfig& cc);
  int Bind(ProxyPtr proxy);
  int Connect(const std::string& conn_id, const std::string& endpoint);
  int Connect(ConnectionConfig& cc);
  SocketPtr GetConnector(const std::string& conn_id);

  int Request(const std::string& conn_id, const std::string& receiver, const std::string& sender, const std::string& message);
  int ReplyTo(Message& hops, const std::string& receiver, const std::string& sender, const std::string& message);

  int AddPublisher(const std::string& pub_id, const std::string& endpoint);
  int AddPublisher(ConnectionConfig& cc);
  int Publish(const std::string& pub_id, const std::string& topic, const std::string& message);
  
  // add subscriber, not connected yet
  int AddSubscriber(const std::string& sub_id, const std::string& endpoint);
  int AddSubscriber(ConnectionConfig& cc);

  int Subscribe(const std::string& sub_id, const std::string& actor_id, const std::string& topics);
  int Subscribe(const std::string& sub_id, const std::string& actor_id, std::vector<std::string>& topics);
  int UnSubscribe(const std::string& sub_id, const std::string& actor_id, const std::string& topic);

  int AddTimer(int delay_ms, int times, const std::string& actor_id);
  int EndTimer(int timer_id, const std::string& actor_id);

  sql::Connection* GetDatabase(const std::string& id);
  kc::PolyDB* GetMemdb(const std::string& id);

  //FIXME(zhangwen): wrap the zmq_pollitem_t
  virtual int OnSocket(zmq_pollitem_t& item);
  virtual int OnTimer(int timer_id);
  virtual void Run();

  void set_db_engine(DatabaseEnginePtr db_engine) { db_engine_ = db_engine; }
  DatabaseEnginePtr db_engine() { return db_engine_; }

  void set_memdb_engine(MemdbEnginePtr memdb_engine) { memdb_engine_ = memdb_engine; }
  MemdbEnginePtr memdb_engine() { return memdb_engine_; }

  std::string id() { return id_; }
  int no() { return no_; }

protected:
  virtual int BeforeLoop() { return 0; }
  virtual int AfterLoop(int retcode) { return 0; }
  virtual int StartLoop();

  void WalkOff();

  void SetSocketOption(SocketPtr sock, ConnectionConfig& cc);

protected:

  ActorMap actors_;
  SocketMap sockets_;
  SocketPtr listener_;
  SocketSet dealers_;
  SocketPtr inner_manager_;

  SocketMap publisher_;

  SocketOptionMap subscribe_options_;
  SocketMap subscriber_;
  SocketActorMap subscribed_actors_;

  TimerActorMap timers_;

  DatabaseEnginePtr db_engine_;
  MemdbEnginePtr memdb_engine_;

  Context& ctx_;
  LoopPtr loop_;
  ProxyPtr proxy_;
  std::string id_;
  int no_;
};
DEFINE_SHARED_PTR(Stage);

} //namespace base
} //namespace ison

#endif   //STAGE_H_
