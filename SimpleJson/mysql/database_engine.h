/* =====================================================================================
 *         Author:  Zhang Wen(zhangwen@szkingdom.com)
 *        Created:  2014-6-15 21:31
 *    Description:  
 * =====================================================================================
 */

#ifndef ISON_BASE_DATABASE_ENGINE_H_
#define ISON_BASE_DATABASE_ENGINE_H_

#include <string>
#include <unordered_map>
#include "db_connection.h"
#include "isonbase.h"

namespace ison {
namespace base {

class ISONBASE_API DatabaseEngine {
public:
  typedef std::unordered_map<std::string, sql::Connection*> ConnMap;

  DatabaseEngine();
  virtual ~DatabaseEngine();
  static sql::Connection* Connect(const char* hostname, const char* username, const char* passwd, const char* schema);

  int Add(const std::string& id, sql::Connection* conn);
  sql::Connection* Get(const std::string& id);

private:
  DISALLOW_COPY_AND_ASSIGN(DatabaseEngine);
  ConnMap all_;
};
DEFINE_SHARED_PTR(DatabaseEngine);

} //namespace base
} //namespace ison

#endif // ISON_BASE_DATABASE_ENGINE_H_