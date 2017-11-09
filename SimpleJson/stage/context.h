/* =====================================================================================
 *         Author:  Zhang Wen(zhangwen@szkingdom.com)
 *        Created:  2014/05/05 16:14:42
 *    Description:  
 * =====================================================================================
 */

#ifndef ISON_BASE_CONTEXT_H_
#define ISON_BASE_CONTEXT_H_

#include "czmq_header.h"
#include "isonbase.h"

namespace ison {
namespace base {

//working with ØMQ contexts
class ISONBASE_API Context {
public:
  Context();
  ~Context();

  void Destroy();

  zctx_t* self() { return self_; }

private:
  DISALLOW_COPY_AND_ASSIGN(Context);
  zctx_t* self_;
};

} //namespace base
} //namespace ison

#endif

