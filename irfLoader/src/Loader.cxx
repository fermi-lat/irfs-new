/**
 * @file Loader.cxx
 *
 * $Header$
 */

#include "dc1Response/loadIrfs.h"
#include "g25Response/loadIrfs.h"

#include <algorithm>
#include <stdexcept>

#include "irfLoader/Loader.h"

namespace {
   char * irf_names[] = {"DC1", "GLAST25"};
}

namespace irfLoader {

std::vector<std::string> Loader::s_irfsNames(::irf_names, ::irf_names+2);

void Loader::go(const std::string & irfsName) {
// @todo Replace this switch with polymorphism or find a way to
// dispatch the desired function call using a map.
   if (irfsName == "DC1") {
      dc1Response::loadIrfs();
   } else if (irfsName == "GLAST25") {
      g25Response::loadIrfs();
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
