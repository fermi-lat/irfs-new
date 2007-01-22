/**
 * @file IrfLoader.h
 * @brief Concrete derived class of irfInterface::IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dc2Response_IrfLoader_h
#define dc2Response_IrfLoader_h

#include "irfInterface/IrfLoader.h"

namespace dc2Response {

/**
 * @class IrfLoader
 * @brief Concrete derived class of irfInterface::IrfLoader
 */

class IrfLoader : public irfInterface::IrfLoader {

public:

   virtual void registerEventClasses() const;

   virtual void loadIrfs() const;

   virtual std::string name() const {
      return "DC2";
   }

};

} // namespace dc2Response

#endif // dc2Response_IrfLoader_h
