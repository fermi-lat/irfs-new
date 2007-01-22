/**
 * @file IrfLoader.cxx
 * @brief Concrete implementation of irfInterface/IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#include "irfInterface/IrfRegistry.h"

#include "dc2Response/IrfLoader.h"

namespace dc2Response {

void load_irfs();

void IrfLoader::registerEventClasses() const {
   irfInterface::IrfRegistry & registry(irfInterface::IrfRegistry::instance());
   const char * dc2_list[] = {"DC2::FrontA", "DC2::BackA", 
                              "DC2::FrontA", "DC2::BackA"};
   registry.registerEventClasses("DC2", dc2_list);
   const char * dc2a_list[] = {"DC2::FrontA", "DC2::BackA"};
   registry.registerEventClasses("DC2_A", dc2a_list);
   registry.registerEventClasses("DC2FA", "DC2::FrontA");
   registry.registerEventClasses("DC2BA", "DC2::BackA");
   registry.registerEventClasses("DC2FB", "DC2::FrontB");
   registry.registerEventClasses("DC2BB", "DC2::BackB");
}

void IrfLoader::loadIrfs() const {
   load_irfs();
}

} // namespace dc2Response
