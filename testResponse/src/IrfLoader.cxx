/**
 * @file IrfLoader.cxx
 * @brief Concrete implementation of irfInterface/IrfLoader
 * @author J. Chiang
 *
 * $Header$
 */

#include "irfInterface/IrfRegistry.h"

#include "testResponse/IrfLoader.h"

namespace testResponse {

void load_irfs();

void IrfLoader::registerEventClasses() const {
   irfInterface::IrfRegistry & registry(irfInterface::IrfRegistry::instance());
   const char * class_names[] = {"testIrfs::Front", "testIrfs::Back"};
   std::vector<std::string> classNames(class_names, class_names + 2);
   registry.registerEventClasses("TEST", classNames);
}

void IrfLoader::loadIrfs() const {
   load_irfs();
}

} // namespace testResponse
