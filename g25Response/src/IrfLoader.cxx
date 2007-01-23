/**
 * @file IrfLoader.cxx
 * @brief Concrete implementation of irfInterface/IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#include "irfInterface/IrfRegistry.h"

#include "g25Response/IrfLoader.h"

namespace g25Response {

void load_irfs();

void IrfLoader::registerEventClasses() const {
   irfInterface::IrfRegistry & registry(irfInterface::IrfRegistry::instance());
   const char * class_names[] = {"Glast25::Front", "Glast25::Back"};
   std::vector<std::string> classNames(class_names, class_names + 2);
   registry.registerEventClasses("G25", classNames);
   registry.registerEventClass("G25F", "Glast25::Front");
   registry.registerEventClass("G25B", "Glast25::Back");
}

void IrfLoader::loadIrfs() const {
   load_irfs();
}

} // namespace g25Response
