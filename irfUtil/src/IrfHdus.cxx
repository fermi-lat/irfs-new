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

IrfHdus::IrfHdus(const std::string & irf_name,
                 const std::string & event_type,
                 const std::vector<std::string> & cnames) {
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

} // namespace latResponse
