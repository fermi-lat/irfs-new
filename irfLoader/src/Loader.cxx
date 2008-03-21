/**
 * @file Loader.cxx
 *
 * $Header$
 */

#include <cstdlib>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "dc1Response/loadIrfs.h"
#include "dc1aResponse/loadIrfs.h"
#include "dc2Response/loadIrfs.h"
#include "g25Response/loadIrfs.h"
#include "testResponse/loadIrfs.h"

#define ST_DLL_EXPORTS
#include "irfLoader/Loader.h"
#undef ST_DLL_EXPORTS

namespace {
   char * irf_names[] = {"DC1", "DC1A", "DC2", "GLAST25", "TEST"};
}

namespace irfLoader {

std::vector<std::string> Loader::s_irfsNames(::irf_names, ::irf_names + 5);

std::map<std::string, std::vector<std::string> > Loader::s_respIds;

void Loader::go(const std::string & irfsName) {
   try {
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
      } else if (irfsName == "DC2" && !s_respIds.count("DC2")) {
         dc2Response::loadIrfs();
         s_respIds["DC2"].clear();
         s_respIds["DC2"].push_back("DC2::FrontA");
         s_respIds["DC2"].push_back("DC2::BackA");
         s_respIds["DC2"].push_back("DC2::FrontB");
         s_respIds["DC2"].push_back("DC2::BackB");
         s_respIds["DC2FA"].clear();
         s_respIds["DC2FA"].push_back("DC2::FrontA");
         s_respIds["DC2BA"].clear();
         s_respIds["DC2BA"].push_back("DC2::BackA");
         s_respIds["DC2FB"].clear();
         s_respIds["DC2FB"].push_back("DC2::FrontB");
         s_respIds["DC2BB"].clear();
         s_respIds["DC2BB"].push_back("DC2::BackB");
         s_respIds["DC2_A"].clear();
         s_respIds["DC2_A"].push_back("DC2::FrontA");
         s_respIds["DC2_A"].push_back("DC2::BackA");
      } else {
         if (!s_respIds.count(irfsName)) {
            throw std::invalid_argument("Request for an invalid set of irfs: "
                                        + irfsName);
         }
      }
   } catch (std::exception & eObj) {
      std::ostringstream message;
      message << eObj.what() << "\n"
              << "Using CALDB = " << ::getenv("CALDB");
      throw std::runtime_error(message.str());
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

void Loader::resetIrfs() {
   s_respIds.clear();
}

} // namespace irfLoader
