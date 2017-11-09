/* =====================================================================================
 *         Author:  Zhang Wen(zhangwen@szkingdom.com)
 *        Created:  2014-9-24 17:35
 *    Description:  
 * =====================================================================================
 */

#ifndef ISON_BASE_PROXY_H_
#define ISON_BASE_PROXY_H_

#include <string>
#include <vector>
#include "czmq_header.h"
#include "isonbase.h"

namespace ison {
namespace base {

class Context;

class ISONBASE_API Proxy {
public:
  Proxy(Context& ctx, const char* frontend, const char* backend);
  ~Proxy();

  int Start();

  Proxy& AddFrontend(const char* frontend);

  size_t GetFrontendSize() const;
  std::string frontend(int i = 0) const;
  std::string backend() const;

private:
  DISALLOW_COPY_AND_ASSIGN(Proxy);
  Context& ctx_;
  zproxy_t* self_;
  std::vector<std::string> frontends_;
  std::string backend_;
};
DEFINE_SHARED_PTR(Proxy);

} //namespace base
} //namespace ison

#endif // ISON_BASE_PROXY_H_