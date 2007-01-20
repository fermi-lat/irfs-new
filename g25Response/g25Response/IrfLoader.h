/**
 * @file IrfLoader.h
 * @brief Concrete derived class of irfLoader::IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef g25Response_IrfLoader_h
#define g25Response_IrfLoader_h

#include "irfLoader/IrfLoader.h"

namespace g25Response {

/**
 * @class IrfLoader
 * @brief Concrete derived class of irfLoader::IrfLoader
 */

class IrfLoader : public irfLoader::IrfLoader {

public:

   virtual void registerEventClasses() const;

   virtual void loadIrfs() const;

   virtual std::string name() const {
      return "G25";
   }

};

} // namespace g25Response

#endif // g25Response_IrfLoader_h
