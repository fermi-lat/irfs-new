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

namespace irfInterface {
   class IAeff;
   class IPsf;
   class IEdisp;
   class IEfficiencyFactor;
}

namespace irfUtil {
   class HdCaldb;
}

namespace latResponse {

/**
 * @class IrfLoader
 * @brief Concrete derived class of irfInterface::IrfLoader
 */

class IrfLoader : public irfInterface::IrfLoader {

public:

   IrfLoader();

   virtual ~IrfLoader();

   virtual void registerEventClasses() const;

   virtual void loadIrfs() const;

   virtual std::string name() const {
      return "LATRESPONSE";
   }

   static void set_edisp_interpolation(bool flag) {
      s_interpolate_edisp = flag;
   }

   static bool interpolate_edisp() {
      return s_interpolate_edisp;
   }

private:
   
   static bool s_interpolate_edisp;

   std::vector<std::string> m_caldbNames;

   std::string m_customIrfDir;

   std::vector<std::string> m_customIrfNames;

   irfUtil::HdCaldb * m_hdcaldb;

   void addIrfs(const std::string & irfName, 
                const std::string & eventType) const;

   void read_caldb_indx();

   void readCustomIrfNames();

   void loadCustomIrfs() const;

   void find_cif(std::string & caldb_indx) const;

   void addIrfs(const std::string & aeff_file,
                const std::string & psf_file,
                const std::string & edisp_file,
                const std::string & irfName,
                const std::string & eventType) const;

   void addIrfs(const std::vector<std::string> & aeff_files,
                const std::vector<std::string> & psf_files,
                const std::vector<std::string> & edisp_files,
                const std::vector<int> & hdus,
                const std::string & irfName,
                const std::string & eventType) const;

   void getCaldbClassNames(const std::string & irfName,
                           const std::string & date="2008-01-30");

   int edispVersion(const std::string & fitsfile) const;

   int psfVersion(const std::string & fitsfile) const;

   irfInterface::IAeff * aeff(const std::string & aeff_file,
                              size_t nrow=0) const;

   irfInterface::IPsf * psf(const std::string & psf_file, bool front,
                            size_t nrow=0) const;

   irfInterface::IEdisp * edisp(const std::string & edisp_file,
                                size_t nrow=0) const;

   irfInterface::IEfficiencyFactor * 
   efficiency_factor(const std::string & aeff_file) const;

};

} // namespace latResponse

#endif // latResponse_IrfLoader_h
