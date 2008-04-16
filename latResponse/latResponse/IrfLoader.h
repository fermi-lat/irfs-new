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

private:
   
   std::vector<std::string> m_caldbNames;

   std::map<std::string, std::vector<std::string> > m_subclasses;

   std::string m_customIrfDir;

   std::vector<std::string> m_customIrfNames;

   void addIrfs(const std::string & version, 
                const std::string & detector,
                int convType, 
                const std::string & date="2008-01-30") const;

   void read_caldb_indx();

   void readCustomIrfNames();

   void loadCustomIrfs() const;

   void find_cif(std::string & caldb_indx) const;

   void addIrfs(const std::string & aeff_file,
                const std::string & psf_file,
                const std::string & edisp_file,
                int convType,
                const std::string & irfName) const;

   const std::vector<std::string> & 
   subclasses(const std::string & irfName) const;

   void getCaldbClassNames(const std::string & irfName,
                           const std::string & date="2008-01-30");
   
   void buildClassNames(const std::string & fitsfile, 
                        const std::string & irfName,
                        std::vector<std::string> & classNames) const;

   static size_t getNumRows(const std::string & fitsfile);

   int edispVersion(const std::string & fitsfile) const;

};

} // namespace latResponse

#endif // latResponse_IrfLoader_h
