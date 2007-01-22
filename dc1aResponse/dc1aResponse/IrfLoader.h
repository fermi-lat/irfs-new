/**
 * @file IrfLoader.h
 * @brief Concrete derived class of irfInterface::IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef dc1aResponse_IrfLoader_h
#define dc1aResponse_IrfLoader_h

#include "irfInterface/IrfLoader.h"

namespace dc1aResponse {

/**
 * @class IrfLoader
 * @brief Concrete derived class of irfInterface::IrfLoader
 */

class IrfLoader : public irfInterface::IrfLoader {

public:

   virtual void registerEventClasses() const;

   virtual void loadIrfs() const;

   virtual std::string name() const {
      return "DC1A";
   }

};

} // namespace dc1aResponse

#endif // dc1aResponse_IrfLoader_h
