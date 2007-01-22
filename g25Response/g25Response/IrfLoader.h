/**
 * @file IrfLoader.h
 * @brief Concrete derived class of irfInterface::IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef g25Response_IrfLoader_h
#define g25Response_IrfLoader_h

#include "irfInterface/IrfLoader.h"

namespace g25Response {

/**
 * @class IrfLoader
 * @brief Concrete derived class of irfInterface::IrfLoader
 */

class IrfLoader : public irfInterface::IrfLoader {

public:

   virtual void registerEventClasses() const;

   virtual void loadIrfs() const;

   virtual std::string name() const {
      return "G25";
   }

};

} // namespace g25Response

#endif // g25Response_IrfLoader_h
