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
   class IrfHdus;
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

   /// @return Single or multi-epoch Aeff.
   static irfInterface::IAeff * aeff(const irfUtil::IrfHdus & aeff_hdus);

   /// @return Single or multi-epoch Psf.
   static irfInterface::IPsf * psf(const irfUtil::IrfHdus & psf_hdus);

   /// @return Single or multi-epoch Edisp.
   static irfInterface::IEdisp * edisp(const irfUtil::IrfHdus & edisp_hdus);

   /// @return Single or multi-epoch EfficiencyFactor.
   static irfInterface::IEfficiencyFactor * 
   efficiency_factor(const irfUtil::IrfHdus & aeff_hdus);

private:
   
   static bool s_interpolate_edisp;

   std::vector<std::string> m_caldbNames;

   std::string m_customIrfDir;

   std::vector<std::string> m_customIrfNames;

   irfUtil::HdCaldb * m_hdcaldb;

   void registerEventClasses(const std::string & irfName) const;

   void loadIrfs(const std::string & irfName) const;

   void addIrfs(const std::string & irf_name, 
                const std::string & event_type) const;

   void read_caldb_indx();

   void readCustomIrfNames();

   void loadCustomIrfs() const;

   void find_cif(std::string & caldb_indx) const;

   static int edispVersion(const std::string & fitsfile,
                           const std::string & extname);

   static int psfVersion(const std::string & fitsfile,
                         const std::string & extname);

   /// @return Single epoch Psf.  Needed to select desired Psf,
   /// Psf2 or Psf3 based on psfVersion return value.
   static irfInterface::IPsf * psf(const irfUtil::IrfHdus & psf_hdus,
                                   size_t iepoch);

   /// @return Single epoch Edisp.  Needed to select Edisp,
   /// Edisp2 or Edisp3 based on edispVersion return value.
   static irfInterface::IEdisp * edisp(const irfUtil::IrfHdus & edisp_hdus,
                                       size_t iepoch);
};

} // namespace latResponse

#endif // latResponse_IrfLoader_h
