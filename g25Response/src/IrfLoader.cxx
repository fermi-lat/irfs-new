/**
 * @file IrfLoader.cxx
 * @brief Concrete implementation of irfLoader/IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#include "irfLoader/IrfLoaderFactory.h"
#include "irfLoader/IrfRegistry.h"

#include "g25Response/IrfLoader.h"

namespace g25Response {

void load_irfs();

void IrfLoader::registerEventClasses() const {
   irfLoader::IrfRegistry & registry(*irfLoader::IrfRegistry::instance());
   std::vector<std::string> classNames;
   classNames.push_back("G25::Front");
   classNames.push_back("G25::Back");
   const char * class_list[] = {"Glast25::Front", "Glast25::Back"};
   registry.registerEventClasses("G25", class_list);
   classNames.pop_back();
   registry.registerEventClasses("G25F", classNames);
   classNames.at(0) = "Glast25::Back";
   registry.registerEventClasses("G25B", classNames);
}

void IrfLoader::loadIrfs() const {
   load_irfs();
}

} // namespace g25Response
