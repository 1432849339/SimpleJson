/* =====================================================================================
 *         Author:  Zhang Wen(zhangwen@szkingdom.com)
 *        Created:  2014-6-18 18:44
 *    Description:  
 * =====================================================================================
 */

#ifndef ISON_BASE_MEMDB_ENGINE_H_
#define ISON_BASE_MEMDB_ENGINE_H_

#include <string>
#include <unordered_map>
#include "memdb_connection.h"
#include "isonbase.h"

namespace ison {
namespace base {

class ISONBASE_API MemdbEngine {
public:
  typedef std::unordered_map<std::string, kc::PolyDB*> MemdbMap;

  MemdbEngine();
  ~MemdbEngine();

  int Add(const std::string& id, kc::PolyDB* db);
  kc::PolyDB* Get(const std::string& id);

  void Release();
private:
  DISALLOW_COPY_AND_ASSIGN(MemdbEngine);
  MemdbMap  all_;
};
DEFINE_SHARED_PTR(MemdbEngine);

} //namespace base
} //namespace ison

#endif // ISON_BASE_MEMDB_ENGINE_H_