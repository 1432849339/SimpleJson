/* =====================================================================================
 *         Author:  Zhang Wen(zhangwen@szkingdom.com)
 *        Created:  2014-7-3 10:05
 *    Description:  
 * =====================================================================================
 */

#ifndef ISON_BASE_IBPROTOCOL_H_
#define ISON_BASE_IBPROTOCOL_H_

#include <string>
#include "isonbase.h"
#include "message.h"

namespace ison {
namespace base {

class ISONBASE_API IBProtocol {
public:
  IBProtocol() {}

  int Parse(Message& message);
  int Build(Message* message, bool reverse = false);

  void GetSender(std::string* sender);
  void SetSender(const std::string& sender);

  void GetReceiver(std::string* receiver);
  void SetReceiver(const std::string& receiver);

  void GetMessage(std::string* message);
  void SetMessage(const std::string& message);

  void GetHops(Message* hops);
  void SetHops(Message& hops);
  bool HasHops();
  void GetClient(std::string* client);

  const std::string& sender() const { return sender_; }
  const std::string& receiver() const { return receiver_; }
  const std::string& message() const { return message_; }
  const Message& hops() const { return hops_; }

private:
  DISALLOW_COPY_AND_ASSIGN(IBProtocol);
  std::string sender_;
  std::string receiver_;
  std::string message_;
  Message hops_;
};

} //namespace base
} //namespace ison

#endif // ISON_BASE_IBPROTOCOL_H_