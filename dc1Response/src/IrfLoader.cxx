/**
 * @file IrfLoader.cxx
 * @brief Concrete implementation of irfLoader/IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#include "irfLoader/IrfLoaderFactory.h"
#include "irfLoader/IrfRegistry.h"

#include "dc1Response/IrfLoader.h"

namespace dc1Response {

void load_irfs();

void IrfLoader::registerEventClasses() const {
   irfLoader::IrfRegistry & registry(*irfLoader::IrfRegistry::instance());
   std::vector<std::string> classNames;
   classNames.push_back("DC1::Front");
   classNames.push_back("DC1::Back");
   const char * class_list[] = {"DC1::Front", "DC1::Back"};
   registry.registerEventClasses("DC1", class_list);
   classNames.pop_back();
   registry.registerEventClasses("DC1F", classNames);
   classNames.at(0) = "DC1::Back";
   registry.registerEventClasses("DC1B", classNames);
}

void IrfLoader::loadIrfs() const {
   load_irfs();
}

} // namespace dc1Response
