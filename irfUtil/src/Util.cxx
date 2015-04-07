/** 
 * @file Util.cxx
 * @brief Implementation for utility class.
 * @author J. Chiang
 * 
 * $Header$
 */

#include <cmath>

#include <iostream>
#include <stdexcept>
#include <utility>

#include "facilities/commonUtilities.h"
#include "facilities/Util.h"

#include "st_facilities/Environment.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "irfUtil/Util.h"
#include "irfUtil/HdCaldb.h"

namespace irfUtil {

void Util::joinPaths(const std::string & components, std::string & final_path) {
                      
   std::vector<std::string> tokens; 
   facilities::Util::stringTokenize(components, " ", tokens);
   if (tokens.size() == 0) {
      throw std::runtime_error("irfUtil::Util: joinPaths: "
                               "empty string input");
   }
   if (tokens.size() == 1) {
      final_path = tokens[0];
      return;
   }
   final_path = facilities::commonUtilities::joinPath(tokens[0], tokens[1]);
   for (size_t i(2); i < tokens.size(); i++) {
      final_path = facilities::commonUtilities::joinPath(final_path,tokens[i]);
   }
   return;
}

void Util::getCaldbFile(const std::string & detName, 
                        const std::string & respName,
                        const std::string & version,
                        std::string & filename,
                        long & extnum,
                        const std::string & telescope,
                        const std::string & instrument,
                        const std::string & filter,
                        const std::string & date,
                        const std::string & time) {
   try {
      HdCaldb caldb_obj(telescope, instrument);
      std::pair<std::string, int> calfile;
      std::string expression = "VERSION.eq." + version;
      calfile = caldb_obj(detName, respName, expression, filter, date, time);
      filename = calfile.first;
      extnum = calfile.second;
   } catch (std::exception & eObj) {
      std::cout << eObj.what() << std::endl;
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
