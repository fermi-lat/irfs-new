/**
 * @file IrfLoader.h
 * @brief Concrete derived class of irfInterface::IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef testResponse_IrfLoader_h
#define testResponse_IrfLoader_h

#include "irfInterface/IrfLoader.h"

namespace testResponse {

/**
 * @class IrfLoader
 * @brief Concrete derived class of irfInterface::IrfLoader
 */

class IrfLoader : public irfInterface::IrfLoader {

public:

   virtual void registerEventClasses() const;

   virtual void loadIrfs() const;

   virtual std::string name() const {
      return "TEST";
   }

};

} // namespace testResponse

#endif // testResponse_IrfLoader_h
