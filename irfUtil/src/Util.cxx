/** 
 * @file Util.cxx
 * @brief Implementation for utility class.
 * @author J. Chiang
 * 
 * $Header$
 */

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

namespace {
void joinPaths(const std::string & components,
               std::string & final_path) {
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

}

namespace irfUtil {

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

void Util::
get_event_class_mapping(std::map<std::string, unsigned int> & mapping) {
   mapping.clear();
   std::string sub_path;
   ::joinPaths("data glast lat bcf irf_index.fits", sub_path);
   std::string irf_index = facilities::commonUtilities::joinPath(
      st_facilities::Environment::getEnv("CALDB"), sub_path);
   const tip::Table * irf_map 
      = tip::IFileSvc::instance().readTable(irf_index, "BITMASK_MAPPING");
   tip::Table::ConstIterator it(irf_map->begin());
   tip::ConstTableRecord & row = *it;
   for ( ; it != irf_map->end(); ++it) {
      std::string event_class;
      int bitpos;
      row["event_class"].get(event_class);
      row["bitposition"].get(bitpos);
      mapping[event_class] = bitpos;
   }
   delete irf_map;
}

void Util::
get_event_type_mapping(const std::string & event_class,
                       std::map<std::string, unsigned int> & mapping) {
   mapping.clear();

   // Find the irf_index.fits file.
   std::string sub_path;
   ::joinPaths("data glast lat bcf irf_index.fits", sub_path);
   std::string irf_index = facilities::commonUtilities::joinPath(
      st_facilities::Environment::getEnv("CALDB"), sub_path);

   // Determine the allowed event_types for this event_class.
   const tip::Table * evclass_map 
      = tip::IFileSvc::instance().readTable(irf_index, "BITMASK_MAPPING");
   tip::Table::ConstIterator it(evclass_map->begin());
   tip::ConstTableRecord & row = *it;
   unsigned int allowed_evtypes;
   std::string my_event_class;
   for ( ; it != evclass_map->end(); ++it) {
      row["event_class"].get(my_event_class);
      if (event_class == my_event_class) {
         row["event_types"].get(allowed_evtypes);
         break;
      }
   }
   delete evclass_map;

   // Create the mapping of event_type name to bit position for the
   // allowed types.
   const tip::Table * evtype_map 
      = tip::IFileSvc::instance().readTable(irf_index, "EVENT_TYPE_MAPPING");
   std::string event_type;
   int bitpos;
   for (it = evtype_map->begin(); it != evtype_map->end(); ++it) {
      row["event_type"].get(event_type);
      row["bitposition"].get(bitpos);
      if ((allowed_evtypes >> bitpos) & 1) {
         mapping[event_type] = bitpos;
      }
   }
   delete evtype_map;
}


} // namespace irfUtil
