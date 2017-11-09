/* =====================================================================================
 *         Author:  Zhang Wen(zhangwen@szkingdom.com)
 *        Created:  2014-6-20 18:42
 *    Description:  
 * =====================================================================================
 */

#ifndef ISON_BASE_IBLOG_H_
#define ISON_BASE_IBLOG_H_

#ifdef _MSC_VER
#define GLOG_NO_ABBREVIATED_SEVERITIES
#endif

#include "glog/logging.h"
#include "glog/raw_logging.h"
#include "isonbase.h"

namespace ison {
namespace base {

class ISONBASE_API IBLog {
public:
  enum {
    kInfo = 0,
    kWarning,
    kError,
    kFatal,
  };

  IBLog();
  IBLog(char* argv0, const char* log_dir = "log");
  ~IBLog();

  void Initialize(char* argv0, const char* log_dir = "log");

  void InstallFailureSignalHandler();

  void LogInfo(int v, const std::string& message);
  void LogError(const std::string& message);
  void LogWarning(const std::string& message);

  int base_verbose() { return base_verbose_; }
  void set_base_verbose(int verbose) { base_verbose_ = verbose; }

  int v();
  void set_v(int v);

  int minloglevel();
  void set_minloglevel(int minloglevel);

  int stderrthreshold();
  void set_stderrthreshold(int stderrthreshold);

  int logbufsecs();
  void set_logbufsecs(int logbufsecs);

  int max_log_size();
  void set_max_log_size(int max_log_size);

  bool is_initialized() { return is_initialized_; }

private:
  DISALLOW_COPY_AND_ASSIGN(IBLog);
  bool is_initialized_;
  int base_verbose_;
};

//If log_dir specified, logfiles are written into this directory instead of the default logging directory.
ISONBASE_API int iblog_init(char* argv0, const char* log_dir = "log");

//Show all VLOG(m) messages for m less or equal the value of this flag
ISONBASE_API int iblog_v(int v);

//Log messages at or above this level. 
//the numbers of severity levels INFO, WARNING, ERROR, and FATAL are 0, 1, 2, and 3, respectively.
ISONBASE_API int iblog_minloglevel(int minloglevel);

//Copy log messages at or above this level to stderr in addition to logfiles. 
//The numbers of severity levels INFO, WARNING, ERROR, and FATAL are 0, 1, 2, and 3, respectively.
ISONBASE_API int iblog_stderrthreshold(int stderrthreshold);

ISONBASE_API int iblog_logbufsecs(int logbufsecs);

ISONBASE_API int iblog_max_log_size(int max_log_size);

} //namespace base
} //namespace ison

#endif // ISON_BASE_IBLOG_H_