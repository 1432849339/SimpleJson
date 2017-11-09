/* =====================================================================================
 *         Author:  Zhang Wen(zhangwen@szkingdom.com)
 *        Created:  2014-6-5 17:51
 *    Description:  
 * =====================================================================================
 */

#ifndef ISON_BASE_H_
#define ISON_BASE_H_

#include <stdint.h>
#include <assert.h>
#include <string>
#include <memory>

namespace ison {
namespace base {

#define IB_VERSION_MAJOR 0
#define IB_VERSION_MINOR 6
#define IB_VERSION_BUILD 2

int ib_version_major();
int ib_version_minor();
int ib_version_build();
int ib_version();
std::string ib_version_string();

#define DEFINE_SHARED_PTR(TypeName) \
  typedef std::shared_ptr<TypeName> TypeName##Ptr

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);                \
  void operator=(const TypeName&)

}
}

#ifdef _MSC_VER
#pragma warning(disable:4251)
# ifdef ISONBASE_EXPORTS
#   define ISONBASE_API _declspec(dllexport)
# elif ISONBASE_IMPORT
#   define ISONBASE_API _declspec(dllimport)
# else
#   define ISONBASE_API
# endif
#else
# define ISONBASE_API
#endif

#endif
