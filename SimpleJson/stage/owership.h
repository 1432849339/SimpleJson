/* =====================================================================================
 *         Author:  Zhang Wen(zhangwen@szkingdom.com)
 *        Created:  2014-6-6 18:53
 *    Description:  
 * =====================================================================================
 */

#ifndef ISON_BASE_OWERSHIP_H_
#define ISON_BASE_OWERSHIP_H_

namespace ison {
namespace base {

class ISONBASE_API OwnerShip {
public:
  OwnerShip() : is_owner_(false) {}
  explicit OwnerShip(bool is_owner) : is_owner_(is_owner) {}
  bool is_owner() { return is_owner_; }
  void set_owner(bool is_owner = true) { is_owner_ = is_owner; }
private:
  bool is_owner_;
};

} //namespace base
} //namespace ison

#endif // ISON_BASE_OWERSHIP_H_