/* =====================================================================================
 *         Author:  Zhang Wen(zhangwen@szkingdom.com)
 *        Created:  2014-8-28 16:05
 *    Description:  
 * =====================================================================================
 */

#ifndef ISON_BASE_UTIL_H_
#define ISON_BASE_UTIL_H_

#include <string>
#include <vector>

namespace ison {
namespace base {

#ifdef _MSC_VER
# ifndef snprintf
#   define snprintf _snprintf
# endif
#endif

int mkdir_p(const char* path);
int split(const std::string& s, const std::string& delim, std::vector< std::string >* ret);
int get_clean_path(const std::string& path, std::string* clean_path);
int get_full_path(const std::string& dir, const std::string& path, std::string* full_path);

} //namespace base
} //namespace ison

#endif // ISON_BASE_UTIL_H_