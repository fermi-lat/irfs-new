/**
 * @file IrfLoader.cxx
 * @brief Concrete implementation of irfInterface/IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#include "irfInterface/IrfRegistry.h"

#include "dc1aResponse/IrfLoader.h"

namespace dc1aResponse {

void load_irfs();

void IrfLoader::registerEventClasses() const {
   irfInterface::IrfRegistry & registry(irfInterface::IrfRegistry::instance());
   const char * class_list[] = {"DC1A::Front", "DC1A::Back"};
   registry.registerEventClasses("DC1A", class_list);
   registry.registerEventClasses("DC1AF", "DC1A::Front");
   registry.registerEventClasses("DC1AB", "DC1A::Back");
}

void IrfLoader::loadIrfs() const {
   load_irfs();
}

} // namespace dc1aResponse
