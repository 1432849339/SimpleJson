/* =====================================================================================
 *         Author:  Zhang Wen(zhangwen@szkingdom.com)
 *        Created:  2014-12-26 11:10
 *    Description:  
 * =====================================================================================
 */

#ifndef ISON_BASE_IBCONFIG_H_
#define ISON_BASE_IBCONFIG_H_

#include <string>
#include <vector>
#include <map>
#include "isonbase.h"
#include "iblog.h"

namespace ison {
namespace base {

struct LogConfig {
  std::string log_dir;
  int v;
  int minloglevel;
  int stderrthreshold;
  int logbufsecs;
  int max_log_size; //in MB
  int base_verbose;

  LogConfig()
    : log_dir("logs"),
    v(0),
    minloglevel(IBLog::kInfo),
    stderrthreshold(IBLog::kError),
    logbufsecs(0),
    max_log_size(1000),
    base_verbose(0) {
  }
};

struct ConnectionConfig {
  std::string id;
  std::string endpoint;

  int sndhwm;
  int rcvhwm;
  int reconnect_ivl;
  int reconnect_ivl_max;
  bool persistent;
  std::string identity;

  ConnectionConfig()
    : sndhwm(1000),
    rcvhwm(1000),
    reconnect_ivl(0),
    reconnect_ivl_max(0),
    persistent(false) {
  }
};

struct ManagerConfig {
  std::vector<ConnectionConfig> listener;
};

struct DataBaseConfig {
  std::string id;
  std::string engine;
  std::string host;
  std::string user;
  std::string passwd;
  std::string schema;
  bool auto_commit;

  DataBaseConfig(): auto_commit(true) {}
};

struct MemDbConfig {
  std::string id;
  std::string engine;
  std::string host;
};

struct StageConfig {
  std::string id;
  std::string script;
  std::string working_dir;
  std::string package_name;
  std::string key;
  
  std::vector<std::string> database;
  std::vector<std::string> memdb;
  
  std::vector<ConnectionConfig> listener;
  std::vector<ConnectionConfig> connector;
  std::vector<ConnectionConfig> publisher;
  std::vector<ConnectionConfig> subscriber;
};

struct IBConfig {
  bool verified;
  bool packaged;
  LogConfig log;
  ManagerConfig manager;
  std::map<std::string, DataBaseConfig> databases;
  std::map<std::string, MemDbConfig> memdbs;
  std::vector<StageConfig> stages;
  
  IBConfig() : verified(false), packaged(false) {
  }
};

} //namespace base
} //namespace ison

#endif // ISON_BASE_IBCONFIG_H_