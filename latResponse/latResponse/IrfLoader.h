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
                       int convType, 
                       std::string irfName="",
                       const std::string & date="2008-01-30");

private:
   
   std::vector<std::string> m_caldbNames;

   std::map<std::string, std::vector<std::string> > m_subclasses;

   std::string m_customIrfDir;

   std::vector<std::string> m_customIrfNames;

   void read_caldb_indx();

   void readCustomIrfNames();

   void loadCustomIrfs() const;

   void find_cif(std::string & caldb_indx) const;

   size_t getNumRows(const std::string & fitsfile) const;

};

} // namespace latResponse

#endif // latResponse_IrfLoader_h
