/**
 * @file IrfLoader.h
 * @brief Concrete derived class of irfLoader::IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef testResponse_IrfLoader_h
#define testResponse_IrfLoader_h

#include "irfLoader/IrfLoader.h"

namespace testResponse {

/**
 * @class IrfLoader
 * @brief Concrete derived class of irfLoader::IrfLoader
 */

class IrfLoader : public irfLoader::IrfLoader {

public:

   virtual void registerEventClasses() const;

   virtual void loadIrfs() const;

   virtual std::string name() const {
      return "testResponse";
   }

};

} // namespace testResponse

#endif // testResponse_IrfLoader_h
