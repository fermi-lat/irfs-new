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

   void addIrfs(const std::string & irf_name, 
                const std::string & event_type) const;

   void read_caldb_indx();

   void readCustomIrfNames();

   void loadCustomIrfs() const;

   void find_cif(std::string & caldb_indx) const;

   void addIrfs(const std::string & irf_name,
                const std::string & event_type) const;

   int edispVersion(const std::string & fitsfile,
                    const std::string & extname) const;

   int psfVersion(const std::string & fitsfile,
                  const std::string & extname) const;

   /// @return Single or multi-epoch Aeff.
   irfInterface::IAeff * aeff(const irfUtil::IrfHdus & aeff_hdus) const;

   /// @return Single or multi-epoch Psf.
   irfInterface::IPsf * psf(const irfUtil::IrfHdus & psf_hdus) const;

   /// @return Single epoch Psf.  Needed to select desired Psf,
   /// Psf2 or Psf3 based on psfVersion return value.
   irfInterface::IPsf * psf(const irfUtil::IrfHdus & psf_hdus,
                            size_t iepoch) const;

   /// @return Single or multi-epoch Edisp.
   irfInterface::IEdisp * edisp(const irfUtil::IrfHdus & edisp_hdus) const;

   /// @return Single epoch Edisp.  Needed to select Edisp,
   /// Edisp2 or Edisp3 based on edispVersion return value.
   irfInterface::IEdisp * edisp(const irfUtil::IrfHdus & edisp_hdus,
                                size_t iepoch) const;

   /// @return Single or multi-epoch EfficiencyFactor.
   irfInterface::IEfficiencyFactor * 
   efficiency_factor(const irfUtil::IrfHdus & aeff_hdus) const;

};

} // namespace latResponse

#endif // latResponse_IrfLoader_h
