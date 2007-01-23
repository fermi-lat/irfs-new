/**
 * @file IrfLoader.cxx
 * @brief Concrete implementation of irfLoader/IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#include "irfInterface/IrfRegistry.h"

#include "dc1Response/IrfLoader.h"

namespace dc1Response {

void load_irfs();

void IrfLoader::registerEventClasses() const {
   irfInterface::IrfRegistry & registry(irfInterface::IrfRegistry::instance());
   const char * class_names[] = {"DC1::Front", "DC1::Back"};
   std::vector<std::string> classNames(class_names, class_names + 2);
   registry.registerEventClasses("DC1", classNames);
   registry.registerEventClass("DC1F", "DC1::Front");
   registry.registerEventClass("DC1B", "DC1::Back");
}

void IrfLoader::loadIrfs() const {
   load_irfs();
}

} // namespace dc1Response
