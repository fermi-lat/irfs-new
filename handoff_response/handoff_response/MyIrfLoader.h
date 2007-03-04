/**
 * @file IrfLoader.h
 * @brief Concrete derived class of irfInterface::IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef handoff_response_IrfLoader_h
#define handoff_response_IrfLoader_h

#include "irfInterface/IrfLoader.h"

namespace handoff_response {

/**
 * @class IrfLoader
 * @brief Concrete derived class of irfInterface::IrfLoader
 */

class MyIrfLoader : public irfInterface::IrfLoader {

public:

   virtual ~MyIrfLoader() {}

   virtual void registerEventClasses() const;

   virtual void loadIrfs() const;

   virtual std::string name() const {
      return "HANDOFF";
   }

private:

   mutable std::string m_className;

};

} // namespace handoff_response

#endif // handoff_response_IrfLoader_h
