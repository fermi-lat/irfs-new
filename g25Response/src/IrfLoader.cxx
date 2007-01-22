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
   const char * class_list[] = {"Glast25::Front", "Glast25::Back"};
   registry.registerEventClasses("G25", class_list);
   registry.registerEventClasses("G25F", "Glast25::Front");
   registry.registerEventClasses("G25B", "Glast25::Back");
}

void IrfLoader::loadIrfs() const {
   load_irfs();
}

} // namespace g25Response
