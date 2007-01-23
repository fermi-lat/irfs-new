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
   std::vector<std::string> dc2List(dc2_list, dc2_list + 4);
   registry.registerEventClasses("DC2", dc2List);
   const char * dc2a_list[] = {"DC2::FrontA", "DC2::BackA"};
   std::vector<std::string> dc2aList(dc2a_list, dc2a_list + 2);
   registry.registerEventClasses("DC2_A", dc2aList);
   registry.registerEventClass("DC2FA", "DC2::FrontA");
   registry.registerEventClass("DC2BA", "DC2::BackA");
   registry.registerEventClass("DC2FB", "DC2::FrontB");
   registry.registerEventClass("DC2BB", "DC2::BackB");
}

void IrfLoader::loadIrfs() const {
   load_irfs();
}

} // namespace dc2Response
