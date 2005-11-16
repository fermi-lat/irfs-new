/**
 * @file Loader.cxx
 *
 * $Header$
 */

#include "dc1Response/loadIrfs.h"
#include "dc1aResponse/loadIrfs.h"
#include "g25Response/loadIrfs.h"
#include "testResponse/loadIrfs.h"
//#include "devResponse/loadIrfs.h"

#include <algorithm>
#include <stdexcept>

#include "irfLoader/Loader.h"

namespace {
//   char * irf_names[] = {"DC1", "GLAST25", "TEST", "DEV"};
   char * irf_names[] = {"DC1", "DC1A", "GLAST25", "TEST"};
}

namespace irfLoader {

std::vector<std::string> Loader::s_irfsNames(::irf_names, ::irf_names + 4);

std::map<std::string, std::vector<std::string> > Loader::s_respIds;

void Loader::go(const std::string & irfsName) {
// @todo Replace this switch with polymorphism or find a way to
// dispatch the desired function call using a map.
   if (irfsName == "DC1" && !s_respIds.count("DC1")) {
      dc1Response::loadIrfs();
      s_respIds["DC1"].clear();
      s_respIds["DC1"].push_back("DC1::Front");
      s_respIds["DC1"].push_back("DC1::Back");
      s_respIds["DC1F"].clear();
      s_respIds["DC1F"].push_back("DC1::Front");
      s_respIds["DC1B"].clear();
      s_respIds["DC1B"].push_back("DC1::Back");
   } else if (irfsName == "GLAST25" && !s_respIds.count("GLAST25")) {
      g25Response::loadIrfs();
      s_respIds["G25"].clear();
      s_respIds["G25"].push_back("Glast25::Front");
      s_respIds["G25"].push_back("Glast25::Back");
      s_respIds["G25F"].clear();
      s_respIds["G25F"].push_back("Glast25::Front");
      s_respIds["G25B"].clear();
      s_respIds["G25B"].push_back("Glast25::Back");
   } else if (irfsName == "TEST" && !s_respIds.count("TEST")) {
      testResponse::loadIrfs();
      s_respIds["TEST"].clear();
      s_respIds["TEST"].push_back("testIrfs::Front");
      s_respIds["TEST"].push_back("testIrfs::Back");
      s_respIds["TESTF"].clear();
      s_respIds["TESTF"].push_back("testIrfs::Front");
      s_respIds["TESTB"].clear();
      s_respIds["TESTB"].push_back("testIrfs::Back");
   } else if (irfsName == "DC1A" && !s_respIds.count("DC1A")) {
      dc1aResponse::loadIrfs();
      s_respIds["DC1A"].clear();
      s_respIds["DC1A"].push_back("DC1A::Front");
      s_respIds["DC1A"].push_back("DC1A::Back");
      s_respIds["DC1AF"].clear();
      s_respIds["DC1AF"].push_back("DC1A::Front");
      s_respIds["DC1AB"].clear();
      s_respIds["DC1AB"].push_back("DC1A::Back");
//    } else if (irfsName == "DEV" && !s_respIds.count("DEV")) {
//       devResponse::loadIrfs();
//       s_respIds["DEV"].push_back("dev::Front");
//       s_respIds["DEV"].push_back("dev::Back");
//       s_respIds["DEVF"].push_back("dev::Front");
//       s_respIds["DEVB"].push_back("dev::Back");
   } else {
      if (!s_respIds.count(irfsName)) {
         throw std::invalid_argument("Request to load an invalid set of irfs: "
                                     + irfsName);
      }
   }
}

void Loader::go(const std::vector<std::string> & irfsNames) {
   std::vector<std::string>::const_iterator name = irfsNames.begin();
   std::vector<std::string> invalidNames;
   for ( ; name != irfsNames.end(); ++name) {
      if (std::find(s_irfsNames.begin(), s_irfsNames.end(), *name) 
          != s_irfsNames.end()) {
         go(*name);
      } else {
         invalidNames.push_back(*name);
      }
   }
   if (invalidNames.size() > 0) {
      std::string message("Request for invalid sets of irfs named: ");
      for (unsigned int i = 0; i < invalidNames.size(); i++) {
         message += invalidNames[i] + "\n";
      }
      throw std::invalid_argument(message);
   }
}

void Loader::go() {
   go(s_irfsNames);
}

} // namespace irfLoader
