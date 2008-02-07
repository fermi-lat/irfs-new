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

   IrfLoader();

   virtual ~IrfLoader() {}

   virtual void registerEventClasses() const;

   virtual void loadIrfs() const;

   virtual std::string name() const {
      return "LATRESPONSE";
   }

   static void addIrfs(const std::string & version, 
                       const std::string & detector,
                       int irfID, 
                       std::string irfName="",
                       const std::string & date="2008-01-30");

private:
   
   std::vector<std::string> m_caldbNames;

   void read_caldb_indx();

};

} // namespace latResponse

#endif // latResponse_IrfLoader_h
