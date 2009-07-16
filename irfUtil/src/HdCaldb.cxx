/**
 * @file HdCaldb.cxx
 * @brief Wrapper class for HEASARC CALDB routines.  (Based on code from
 * Goodi::FitsService by S. Bansal.)
 *
 * @author J. Chiang
 *
 * $Header$
 */

#include <stdexcept>

#include "facilities/commonUtilities.h"
#include "st_facilities/Util.h"

#include "Hdcal.h"

#include "irfUtil/HdCaldb.h"

namespace irfUtil {

HdCaldb::HdCaldb(const std::string & telescope, const std::string & instrument)
   : m_telescope(telescope), m_instrument(instrument), m_filenamesize(256) {

// Check that CALDB files exist.
   st_facilities::Util::file_ok(
      facilities::commonUtilities::getEnvironment("CALDBALIAS"));
   st_facilities::Util::file_ok(
      facilities::commonUtilities::getEnvironment("CALDBCONFIG"));

   for (int i = 0; i < s_maxret; i++) {
      m_filenames[i] = new char[m_filenamesize];
      m_online[i] = new char[m_filenamesize];
   }
}

HdCaldb::~HdCaldb() {
   for (int i = 0; i < s_maxret; i++) {
      delete [] m_filenames[i];
      delete [] m_online[i];
   }
}

std::pair<std::string, int> 
HdCaldb::operator()(const std::string & detName, 
                    const std::string & respName,
                    const std::string & expression, 
                    const std::string & filter,
                    const std::string & date, 
                    const std::string & time) {
   int nret;
   int nfound(0);
   int status(0);

// These should probably be passed as arguments, but for now it suffices
// to set their values with these place holders.
   std::string startdate = date;
   std::string starttime = time;
   std::string stopdate = date;
   std::string stoptime = time;

   HDgtcalf(m_telescope.c_str(), m_instrument.c_str(),
            detName.c_str(), filter.c_str(), respName.c_str(),
            startdate.c_str(), starttime.c_str(), 
            stopdate.c_str(), stoptime.c_str(), expression.c_str(),
            s_maxret, m_filenamesize, m_filenames, m_extnums, 
            m_online, &nret, &nfound, &status);

   if (status != 0 || nfound == 0) {
      throw std::runtime_error(std::string("irfUtil::HdCaldb:\n")
                               + "Error locating CALDB file.");
   }
   std::string filename = m_filenames[0];
   int extnum = m_extnums[0];

   return std::make_pair(filename, extnum);
}

} // namespace irfUtil
