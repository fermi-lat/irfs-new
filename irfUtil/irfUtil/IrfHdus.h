/**
 * @file IrfHdus.h
 * @brief Container for the HDU numbers associated with specified
 * IRF components.
 * @author J. Chiang
 * 
 * $Header$
 */

#ifndef _irfUtil_IrfHdus_h
#define _irfUtil_IrfHdus_h

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace irfUtil {

class IrfHdus {

public:

   /// @param irf_name IRF name, e.g., "P8_SOURCE_V5"
   /// @param event_type Event type, e.g., "FRONT", "PSF0"
   /// @param cnames Vector of IRF component names, e.g., 
   ///        {"EFF_AREA", "PHI_DEP", "EFFICIENCY_PARS"}
   IrfHdus(const std::string & irf_name,
           const std::string & event_type,
           const std::vector<std::string> & cnames);

   typedef std::vector< std::pair<std::string, std::string> >
   FilenameHduPairs_t;

   /// @return Vector of (filename, HUD number (cast as string)) pairs.
   ///         This is a vector to accommodate epoch dependent IRFs.
   /// @param cname IRF component name
   const FilenameHduPairs_t & operator()(const std::string & cname) const;
   
   /// @return Number of epochs.
   size_t numEpochs() const;

   /// @return The CNAM values for this IRF component.
   const std::vector<std::string> & cnames() const;

   // @brief Factory methods for the three IRF components.
   static IrfHdus aeff(const std::string & irf_name,
                       const std::string & event_type);
   static IrfHdus psf(const std::string & irf_name,
                      const std::string & event_type);
   static IrfHdus edisp(const std::string & irf_name,
                        const std::string & event_type);

   /// @brief cnames for the three IRF components.
   static std::vector<std::string> s_aeff_cnames;
   static std::vector<std::string> s_psf_cnames;
   static std::vector<std::string> s_edisp_cnames;

private:

   const std::vector<std::string> & m_cnames;

   std::map<std::string, FilenameHduPairs_t> m_file_hdus;

};

} // namespace irfUtil

#endif // _irfUtil_IrfHdus_h
