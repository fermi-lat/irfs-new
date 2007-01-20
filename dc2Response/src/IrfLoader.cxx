/**
 * @file IrfLoader.cxx
 * @brief Concrete implementation of irfLoader/IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#include "irfLoader/IrfLoaderFactory.h"
#include "irfLoader/IrfRegistry.h"

#include "dc2Response/IrfLoader.h"

namespace dc2Response {

void load_irfs();

void IrfLoader::registerEventClasses() const {
   irfLoader::IrfRegistry & registry(*irfLoader::IrfRegistry::instance());
   const char * dc2_list[] = {"DC2::FrontA", "DC2::BackA", 
                              "DC2::FrontA", "DC2::BackA"};
   registry.registerEventClasses("DC2", dc2_list);
   const char * dc2a_list[] = {"DC2::FrontA", "DC2::BackA"};
   registry.registerEventClasses("DC2_A", dc2a_list);
   const char * dc2fa[] = {"DC2::FrontA"};
   registry.registerEventClasses("DC2FA", dc2fa);
   const char * dc2ba[] = {"DC2::BackA"};
   registry.registerEventClasses("DC2BA", dc2ba);
   const char * dc2fb[] = {"DC2::FrontB"};
   registry.registerEventClasses("DC2FB", dc2fb);
   const char * dc2bb[] = {"DC2::BackB"};
   registry.registerEventClasses("DC2BB", dc2bb);
}

void IrfLoader::loadIrfs() const {
   load_irfs();
}

} // namespace dc2Response
