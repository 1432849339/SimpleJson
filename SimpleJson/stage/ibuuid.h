/* =====================================================================================
 *         Author:  Zhang Wen(zhangwen@szkingdom.com)
 *        Created:  2014-6-25 9:23
 *    Description:  
 * =====================================================================================
 */

#ifndef ISON_BASE_IBUUID_H_
#define ISON_BASE_IBUUID_H_

#include "czmq_header.h"
#include "isonbase.h"

namespace ison {
namespace base {

class ISONBASE_API IBUUID {
public:
  //  Constructor
  IBUUID();
  explicit IBUUID(zuuid_t* self);

  //  Destructor
  ~IBUUID();

  //  Return UUID binary data
  uint8_t* Data();

  //  Return UUID binary size
  size_t Size();

  //  Returns UUID as string
  char* Str();

  //  Set UUID to new supplied value 
  void Set(uint8_t* source);

  //  Store UUID blob in target array
  void Export(uint8_t* target);

  //  Check if UUID is same as supplied value
  bool Equal(uint8_t* compare);

  //  Check if UUID is different from supplied value
  bool NoEqual(uint8_t* compare);

  //  Make copy of UUID object
  IBUUID* Dup(zuuid_t *self);
private:
  zuuid_t* self_;
};

} //namespace base
} //namespace ison

#endif // ISON_BASE_IBUUID_H_