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

#include "irfLoader/IrfLoaderFactoryBase.h"
#include "irfLoader/IrfRegistry.h"

#define ST_DLL_EXPORTS
#include "irfLoader/Loader.h"
#undef ST_DLL_EXPORTS

namespace irfLoader {

std::map<std::string, std::vector<std::string> > Loader::s_respIds;

void Loader::go(const std::string & name) {
   irfLoader::IrfRegistry & registry(*irfLoader::IrfRegistry::instance());
   std::vector<std::string> irfNames(registry.irfNames());
   if (std::find(irfNames.begin(), irfNames.end(), name) != irfNames.end()) {
      s_respIds[name] = registry[name];
      registry.loadIrfs(name);
   }
   throw std::invalid_argument("Invalid IRF named " + name + ".");
}

void Loader::go(const std::vector<std::string> & irfsNames) {
   for (std::vector<std::string>::const_iterator name(irfsNames.begin());
        name != irfsNames.end(); ++name) {
      go(*name);
   }
}

void Loader::go() {
   irfLoader::IrfRegistry & registry(*irfLoader::IrfRegistry::instance());
   std::vector<std::string> names(registry.irfNames());
//   std::cout << "Loadable irfs: " << std::endl;
   for (size_t i(0); i < names.size(); i++) {
//      std::cout << names.at(i) << std::endl;
      s_respIds[names.at(i)] = registry[names.at(i)];
      registry.loadIrfs(names.at(i));
   }
}

void Loader::resetIrfs() {
   s_respIds.clear();
}

void Loader_go() {
   Loader::go();
}

const std::map<std::string, std::vector<std::string> > & Loader_respIds() {
   return Loader::respIds();
}

} // namespace irfLoader
