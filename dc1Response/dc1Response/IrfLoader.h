/**
 * @file IrfLoader.h
 * @brief Concrete derived class of irfLoader::IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dc1Response_IrfLoader_h
#define dc1Response_IrfLoader_h

#include "irfInterface/IrfLoader.h"

namespace dc1Response {

/**
 * @class IrfLoader
 * @brief Concrete derived class of irfInterface::IrfLoader
 */

class IrfLoader : public irfInterface::IrfLoader {

public:

   virtual void registerEventClasses() const;

   virtual void loadIrfs() const;

   virtual std::string name() const {
      return "DC1";
   }

};

} // namespace dc1Response

#endif // dc1Response_IrfLoader_h
