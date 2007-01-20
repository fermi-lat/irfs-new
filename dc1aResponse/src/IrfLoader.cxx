/**
 * @file IrfLoader.cxx
 * @brief Concrete implementation of irfLoader/IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#include "irfLoader/IrfLoaderFactory.h"
#include "irfLoader/IrfRegistry.h"

#include "dc1aResponse/IrfLoader.h"

namespace dc1aResponse {

void load_irfs();

void IrfLoader::registerEventClasses() const {
   irfLoader::IrfRegistry & registry(*irfLoader::IrfRegistry::instance());
   std::vector<std::string> classNames;
   classNames.push_back("DC1A::Front");
   classNames.push_back("DC1A::Back");
   const char * class_list[] = {"DC1A::Front", "DC1A::Back"};
   registry.registerEventClasses("DC1A", class_list);
   classNames.pop_back();
   registry.registerEventClasses("DC1AF", classNames);
   classNames.at(0) = "DC1A::Back";
   registry.registerEventClasses("DC1AB", classNames);
}

void IrfLoader::loadIrfs() const {
   load_irfs();
}

} // namespace dc1aResponse
