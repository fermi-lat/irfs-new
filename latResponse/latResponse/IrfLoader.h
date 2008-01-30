/**
 * @file IrfLoader.h
 * @brief Concrete derived class of irfInterface::IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef latResponse_IrfLoader_h
#define latResponse_IrfLoader_h

#include "irfInterface/IrfLoader.h"

namespace latResponse {

/**
 * @class IrfLoader
 * @brief Concrete derived class of irfInterface::IrfLoader
 */

class IrfLoader : public irfInterface::IrfLoader {

public:

   virtual ~IrfLoader() {}

   virtual void registerEventClasses() const;

   virtual void loadIrfs() const;

   virtual std::string name() const {
      return "HANDOFF";
   }

   static void addIrfs(const std::string & version, 
                       const std::string & detector,
                       int irfID, 
                       std::string irfName="",
                       const std::string & date="2008-01-30");

private:
   
   mutable std::string m_className;

};

} // namespace latResponse

#endif // latResponse_IrfLoader_h
