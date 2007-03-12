/**
 * @file Loader.cxx
 *
 * $Header$
 */

#include "dc1Response/loadIrfs.h"
#include "g25Response/loadIrfs.h"
#include "testResponse/loadIrfs.h"

#include <algorithm>
#include <stdexcept>

#include "irfLoader/Loader.h"

namespace {
   char * irf_names[] = {"DC1", "GLAST25", "TEST"};
}

namespace irfLoader {

std::vector<std::string> Loader::s_irfsNames(::irf_names, ::irf_names+3);

std::map<std::string, std::vector<std::string> > Loader::s_respIds;

void Loader::go(const std::string & irfsName) {
// @todo Replace this switch with polymorphism or find a way to
// dispatch the desired function call using a map.
   if (irfsName == "DC1") {
      dc1Response::loadIrfs();
      s_respIds["DC1"].push_back("DC1::Front");
      s_respIds["DC1"].push_back("DC1::Back");
      s_respIds["DC1F"].push_back("DC1::Front");
      s_respIds["DC1B"].push_back("DC1::Back");
// These are deprecated in favor of "DC1" versions:
      s_respIds["FRONT/BACK"].push_back("DC1::Front");
      s_respIds["FRONT/BACK"].push_back("DC1::Back");
      s_respIds["FRONT"].push_back("DC1::Front");
      s_respIds["BACK"].push_back("DC1::Back");
   } else if (irfsName == "GLAST25") {
      g25Response::loadIrfs();
      s_respIds["G25"].push_back("Glast25::Front");
      s_respIds["G25"].push_back("Glast25::Back");
      s_respIds["G25F"].push_back("Glast25::Front");
      s_respIds["G25B"].push_back("Glast25::Back");
// Deprecated:
      s_respIds["GLAST25"].push_back("Glast25::Front");
      s_respIds["GLAST25"].push_back("Glast25::Back");
      s_respIds["G25FRONT"].push_back("Glast25::Front");
      s_respIds["G25BACK"].push_back("Glast25::Back");
   } else if (irfsName == "TEST") {
      testResponse::loadIrfs();
      s_respIds["TEST"].push_back("testIrfs::Front");
      s_respIds["TEST"].push_back("testIrfs::Back");
      s_respIds["TESTF"].push_back("testIrfs::Front");
      s_respIds["TESTB"].push_back("testIrfs::Back");
   } else {
      throw std::invalid_argument("Request for an invalid set of irfs named "
                                  + irfsName);
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
