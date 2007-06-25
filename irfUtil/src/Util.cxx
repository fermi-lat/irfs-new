/** 
 * @file Util.cxx
 * @brief Implementation for utility class.
 * @author J. Chiang
 * 
 * $Header$
 */

#include <cassert>
#include <cmath>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "fitsio.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "irfUtil/Util.h"
#include "irfUtil/HdCaldb.h"

namespace irfUtil {

void Util::getCaldbFile(const std::string &detName, 
                        const std::string &respName,
                        const std::string &version,
                        std::string &filename, long &extnum,
                        const std::string & telescope,
                        const std::string & instrument) {
   std::string filter("NONE");
   std::string date("2003-01-01");
   std::string time("00:00:00");
   try {
      std::string expression = "VERSION.eq." + version;
      std::pair<std::string, int> calfile;
      HdCaldb caldb_obj(telescope, instrument);
      calfile = caldb_obj(detName, respName, expression);
      filename = calfile.first;
      extnum = calfile.second + 1;
   } catch (...) {
      std::cout << "irfUtil::Util::getCaldbFile: \n"
                << "Error trying to find filename and extension for \n"
                << "   detName: " << detName
                << "   respName: " << respName
                << "   version: " << version << "\n"
                << "Try checking your CALDB environment variable."
                << std::endl;
      throw;
   }
}

} // namespace irfUtil
