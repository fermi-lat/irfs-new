/**
 * @file IrfHdus.cxx
 * @brief Container for the HDU numbers associated with specified
 * IRF components.
 * @author J. Chiang
 *
 * $Header$
 */

#include <sstream>
#include <stdexcept>

#include "irfUtil/HdCaldb.h"
#include "irfUtil/IrfHdus.h"

namespace irfUtil {

const char * aeff_cnames[3] = {"EFF_AREA", "PHI_DEP", "EFFICIENCY_PARS"};
const char * psf_cnames[3] = {"RPSF", "PSF_SCALING", "FISHEYE_CORR"};
const char * edisp_cnames[2] = {"EDISP", "EDISP_SCALING"};

std::vector<std::string> IrfHdus::s_aeff_cnames(aeff_cnames, aeff_cnames+3);
                                                
std::vector<std::string> IrfHdus::s_psf_cnames(psf_cnames, psf_cnames+2);
                                               
std::vector<std::string> IrfHdus::s_edisp_cnames(edisp_cnames, edisp_cnames+2);

IrfHdus::IrfHdus(const std::string & irf_name,
                 const std::string & event_type,
                 const std::vector<std::string> & cnames) 
   : m_cnames(cnames) {
   irfUtil::HdCaldb hdcaldb("GLAST", "LAT");

   for (size_t i(0); i < cnames.size(); i++) {
      std::vector<std::string> filenames;
      std::vector<int> hdus;
      hdcaldb.getFiles(filenames, hdus, event_type, cnames[i], irf_name);
      FilenameHduPairs_t fh_pairs;
      for (size_t j(0); j < hdus.size(); j++) {
         std::ostringstream extname;
         extname << hdus[j];
         fh_pairs.push_back(std::make_pair(filenames.at(j), extname.str()));
      }
      m_file_hdus[cnames[i]] = fh_pairs;
   }
}

typedef std::vector< std::pair<std::string, std::string> > FilenameHduPairs_t;

const FilenameHduPairs_t & 
IrfHdus::operator()(const std::string & cname) const {
   const std::map<std::string, FilenameHduPairs_t>::const_iterator 
      it(m_file_hdus.find(cname));
   if (it == m_file_hdus.end()) {
      throw std::runtime_error("latResponse::IrfHdus: " + cname 
                               + " not found.");
   }
   return it->second;
}

size_t IrfHdus::numEpochs() const {
   return m_file_hdus.begin()->second.size();
}

const std::vector<std::string> & IrfHdus::cnames() const {
   return m_cnames;
}

IrfHdus IrfHdus::aeff(const std::string & irf_name,
                      const std::string & event_type) {
   return IrfHdus(irf_name, event_type, s_aeff_cnames);
}

IrfHdus IrfHdus::psf(const std::string & irf_name,
                     const std::string & event_type) {
   return IrfHdus(irf_name, event_type, s_psf_cnames);
}

IrfHdus IrfHdus::edisp(const std::string & irf_name,
                       const std::string & event_type) {
   return IrfHdus(irf_name, event_type, s_edisp_cnames);
}

} // namespace latResponse
